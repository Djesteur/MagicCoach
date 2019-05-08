import contractions
import inflect
import json
import re
import requests
import nltk
import unicodedata
from mtgsdk import Card
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
from nltk.stem import WordNetLemmatizer
from word2number import w2n

nltk.download("stopwords")


# Clean up the text, normalizing it
def normalize(cardText):
    if (cardText is not None):
        # Removing non ASCII characters
        cardText = unicodedata.normalize(
            'NFKD', cardText).encode("ascii", "ignore").decode("utf-8", "ignore")

        # Removing contractions
        cardText = contractions.fix(cardText, slang=False)

        # Tokenizing
        tokenizedText = word_tokenize(cardText)

        # Putting all words in lowercase
        tokenizedText = [word.lower() for word in tokenizedText]

        # Deleting ponctuations
        tokenizedText = [word for word in tokenizedText if word.isalpha()]

        # Removing stop words
        tokenizedText = [
            word for word in tokenizedText if not word in stopwords.words("english")]

        # Converting numbers
        inflectEngine = inflect.engine()
        newWords = []
        for word in tokenizedText:
            if word.isdigit():
                newWords.append(inflectEngine.number_to_words(word))
            else:
                newWords.append(word)
        tokenizedText = newWords

        # Lemmatization
        lemmatizer = WordNetLemmatizer()
        cardText = [lemmatizer.lemmatize(word, pos='v')
                    for word in tokenizedText]

        # Turning it back into a string
        cardText = "".join([" " + i for i in tokenizedText]).strip()

        return cardText

    else:  # If the text is empty
        return ""


# Returns the card's note about its control capacities:
# Ability to counter,
# Stops the opponent from playing,
# Ability to destroy
# Defender keyword
def control(card):
    if ((card["oracle_text"].find("counter ") != -1
            and re.search("[+-][0-9]/[+-][0-9] counter ", card["oracle_text"]) is None)
            or card["oracle_text"].find("destroy") != -1
            or card["oracle_text"].find("defender") != -1):
        return 1

    return 0


# Returns the card's note about its tempo capacities:
# Ability to buy time
def tempo(card):
    if (card["oracle_text"].find("tap target") != -1):
        return 1

    return 0


# Returns the card's note about its polyvalence capacities:
# Adapted to different contexts
def polyvalence(card):
    if (card["oracle_text"].find("choose one") != -1):
        return 1

    return 0


# Returns the card's note about its creature capacities:
# Ratio of (self stats + stats of created tokens) / converted mana cost
# Ignoring values of power and toughness changing depending on the state of the game
def creature(card):
    statsSum = 0

    tokenText = re.search(
        "create (one|two|three|four|five|six|seven|eight|nine) ([0-9])/([0-9]) .+ token", card["oracle_text"])

    if tokenText is not None:
        tokenNumber = int(w2n.word_to_num(tokenText.group(1)))
        tokenStats = int(tokenText.group(2)) + int(tokenText.group(3))

        statsSum += tokenNumber * tokenStats

    if (card["power"] is not None
            and card["power"] != "*"):
        statsSum += int(card["power"])

    if (card["toughness"] is not None
            and card["toughness"] != "*"):
        statsSum += int(card["toughness"])

    if (card["cmc"] != 0):
        return statsSum / card["cmc"]
    else:
        return statsSum


# Returns the card's note about its value capacities:
# Ability to draw cards
def value(card):
    if (card["oracle_text"].find("draw ") != -1):
        return 1

    return 0


# Returns the card's note about its boost capacities:
# Ability to improve cards, permanently or not
def boost(card):
    if (card["oracle_text"].find("gains") != -1
            or re.search("\+[0-9]/\+[0-9]", card["oracle_text"]) is not None):
        return 1

    return 0


# Returns the card's note about its capacities in all categories
def analyseCard(card):
    #card["oracle_text"] = normalize(card["oracle_text"])

    return {
        "Control": control(card),
        "Tempo": tempo(card),
        "Polyvalence": polyvalence(card),
        "Creature": creature(card),
        "Value": value(card),
        "Boost": boost(card)
    }


# Creates a .json file where all data about cards are stored
def createJson(cardsList):
    serialisedCard = []

    for card in cardsList:
        if "mana_cost" not in card:
            card.update({"mana_cost": None})
        if "color_identity" not in card:
            card.update({"color_identity": None})
        if "oracle_text" not in card:
            card.update({"oracle_text": ""})
        if "power" not in card:
            card.update({"power": None})
        if "toughness" not in card:
            card.update({"toughness": None})
        if "loyalty" not in card:
                card.update({"loyalty": None})

        serialisedCard.append(
            {
                card["arena_id"]:
                {
                    "Name": card["name"],
                    "Mana cost": card["mana_cost"],
                    "Color identity": card["color_identity"],
                    "Type": card["type_line"],
                    "Text": card["oracle_text"],
                    "Power": card["power"],
                    "Toughness": card["toughness"],
                    "Loyalty": card["loyalty"],
                    "Categories": analyseCard(card)
                }
            }
        )

    with open('cardsList.json', 'w', encoding='utf-8') as outfile:
        json.dump(serialisedCard, outfile, sort_keys=True,
                  indent=4, ensure_ascii=False)


# Main function
def main():
    print("Classifying cards...")

    pageCount = 1
    request = requests.get(
        url="https://api.scryfall.com/cards/search?page=" + str(pageCount) + "&q=game%3Aarena&unique=cards")
    cardsData = request.json()

    while cardsData['has_more']:
        createJson(cardsData["data"])

        pageCount += 1
        request = requests.get(
            url="https://api.scryfall.com/cards/search?page=" + str(pageCount) + "&q=game%3Aarena&unique=cards")
        cardsData = request.json()

    print("Cards classified")


if (__name__ == "__main__"):
    main()

import json
import re
from mtgsdk import Card
import nltk
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
from nltk.stem import WordNetLemmatizer
import unicodedata
import contractions

nltk.download("stopwords")


# Clean up the text, normalizing it
def normalize(cardText):
    if (cardText is not None):
        # Removing non ASCII characters
        #cardText = unicodedata.normalize(
        #    'NFKD', cardText).encode("ascii", "ignore")

        # Removing contractions
        cardText = contractions.fix(cardText, slang = False)

        # Tokenizing
        tokenizedText = word_tokenize(cardText)

        # Putting all words in lowercase
        tokenizedText = [word.lower() for word in tokenizedText]

        # Deleting ponctuations
        tokenizedText = [word for word in tokenizedText if word.isalpha()]

        # Removing stop words
        stopWords = stopwords.words('english')
        tokenizedText = [word for word in tokenizedText if not word in stopWords]

        # Lemmatization (or Stemmatization?)
        #lemmatizer = WordNetLemmatizer()
        #cardText = [lemmatizer.lemmatize(word, pos = 'v') for word in tokenizedText]

        # Turn it back into a string
        cardText = "".join([" " + i for i in tokenizedText]).strip()
        print(cardText)

        return cardText

    else: # If the text is empty
        return ""

# Returns the card's note about its control capacities:
# Ability to counter,
# Stops the opponent from playing
# Creature management


def control(card):
    if (card.text.find("counter") != -1
            and re.search("[+-][0-9]/[+-][0-9] counter", card.text) is None):
        return 1

    return 0

# Returns the card's note about its tempo capacities:
# Ability to buy time


def tempo(card):
    return -1

# Returns the card's note about its polyvalence capacities:
# Adapted to different contexts


def polyvalence(card):
    if (card.text.find("choose one") != -1):
        return 1

    return 0

# Returns the card's note about its creature capacities:
# Ratio of (stats + created tokens) / converted mana cost


def creature(card):
    statsSum = 0

    tokenText = re.search(
        "create .+ [0-9]/[0-9] .+ token", card.text)

    if (tokenText is not None):
        tokenNumber = re.search("one|two|three|four|five|six|seven|eight|nine", tokenText)
        tokenStats = re.findall("[0-9]", tokenText.group())

        for tokenStat in tokenStats:
            statsSum += int(tokenStat)

    if (card.power is not None
            and card.power != "*"
            and card.toughness != "*"):
        statsSum += int(card.power) + int(card.toughness)

    if (card.cmc != 0):
        return statsSum / card.cmc
    else:
        return statsSum

# Returns the card's note about its value capacities:
# Ability to draw cards


def value(card):
    if (card.text.find("draw") != -1):
        return 1

    return 0

# Returns the card's note about its boost capacities:
# Ability to improve cards, permanently or not


def boost(card):
    if (card.text.find("gains") != -1
            or re.search("\+[0-9]/\+[0-9]", card.text) is not None):
        return 1

    return 0

# Returns the card's note about its capacities in all categories


def analyseCard(card):
    if card.text is None:
        card.text = ""
    #card.text = normalize(card.text)

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
    serialisedCard = {}

    for card in cardsList:
        serialisedCard.update(
            {
                card.name:
                {
                    "Mana cost": card.mana_cost,
                    "Color identity": card.color_identity,
                    "Type": card.type,
                    "Loyalty": card.loyalty,
                    "Power": card.power,
                    "Toughness": card.toughness,
                    "Text": card.text,
                    "Categories": analyseCard(card)
                }
            }
        )

    with open('cardsList.json', 'w') as outfile:
        json.dump(serialisedCard, outfile, sort_keys=True,
                  indent=4, ensure_ascii=False)

# Main function


def main():
    print("Retrieving cards...")
    mtgaCards = Card.where(gameFormat="Standard").all()
    print("Cards retrieved")

    print("Classifying cards...")
    createJson(mtgaCards)
    print("Cards classified")


if (__name__ == "__main__"):
    main()

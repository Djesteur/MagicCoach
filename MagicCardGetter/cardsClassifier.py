import json
import re
from mtgsdk import Card
#from nltk.corpus import stopwords
#from sklearn.feature_extraction.text import CountVectorizer
# vectorizer = CountVectorizer(stop_words=stopwords.words('english'))
# X = vectorizer.fit_transform([card.text]).toarray()
# print(card.name, ": ", X.shape)


# Returns the card's note about its control capacities:
# Countering cards,
# Stops the opponent from playing
# Creature management


def control(card):
    if (card.text is not None
            and card.text.lower().find("counter") != -1
            and re.search("[+-][0-9]/[+-][0-9] counter", card.text.lower()) is None):
        return 1

    return 0

# Returns the card's note about its tempo capacities:
# Buying time


def tempo(card):
    return -1

# Returns the card's note about its polyvalence capacities:
# Adapted to different contexts


def polyvalence(card):
    if (card.text is not None
            and card.text.lower().find("choose one") != -1):
        return 1

    return 0

# Returns the card's note about its creature capacities:
# Ratio of mana cost / (stats + created tokens)


def creature(card):
    sum = 0

    if (card.text is not None):
        tokenText = re.search(
            "create .+ [0-9]/[0-9] .+ token", card.text.lower())

        if (tokenText is not None):
            tokenStats = re.findall("[0-9]", tokenText.group())

            for tokenStat in tokenStats:
                sum += int(tokenStat)

    if (card.power is not None
            and card.power != "*"
            and card.toughness != "*"):
        sum += int(card.power) + int(card.toughness)

    if (sum >= card.cmc * 2): #TODO Get a real ratio
        return 1

    return 0

# Returns the card's note about its value capacities:
# Drawing cards


def value(card):
    if (card.text is not None
            and card.text.lower().find("draw") != -1):
        return 1

    return 0

# Returns the card's note about its boost capacities:
# Improving cards, permanently or not


def boost(card):
    if (card.text is not None
            and (card.text.lower().find("gains") != -1
                 or re.search("\+[0-9]/\+[0-9]", card.text.lower()) is not None)):
        return 1

    return 0

# Returns the card's note about its capacities in all categories


def analyseCard(card):
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

import json
import re
import sys
from word2number import w2n


#############
# ABILITIES #
#############


# Superclass for all Abilities
class Ability:
    def __init__(self, gameState, card):
        self.gameState = gameState
        self.card = card
    
    def activate(self):
        pass

    def action(self):
        pass


######################
# ABILITY CATEGORIES #
######################

# Ability that requires some sort of cost
class ActivatedAbility(Ability):
    def __init__(self, gameState, card):
        Ability.__init__(self, gameState, card)

    def activate(self):
        self.payCost()
        self.action()

    def payCost(self):
        pass


# Ability that requires a specific event to happen
class TriggeredAbility(Ability):
    def __init__(self, gameState, card):
        Ability.__init__(self, gameState, card)

    def activate(self):
        if (self.checkEvent()):
            self.action()

    def checkEvent(self):
        pass


# Ability with continuous effect
class StaticAbility(Ability):
    def __init__(self, gameState, card):
        Ability.__init__(self, gameState, card)

    def activate(self):
        self.action()


# Ability from spell
class SpellAbility(Ability):
    def __init__(self, gameState, card):
        Ability.__init__(self, gameState, card)

    def activate(self):
        self.action()


#######################
# EVERGREEN ABILITIES #
#######################

# Any amount of damage dealt is enough to kill the receiving creature
class Deathtouch(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        self.card.target.destroy()


# Prohibits the creature from attacking
class Defender(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets the creature deal its combat damage twice
class DoubleStrike(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Defines what an Aura spell can target and what an Aura permanent can be attached to
class Enchant(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets a player attach an Equipment to a creature they control
class Equip(ActivatedAbility):
    def __init__(self, gameState, card):
        ActivatedAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets a creature deal its combat damage before other creatures
class FirstStrike(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets a player play a card any time they could cast an instant
class Flash(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Restricts how a creature may be blocked
class Flying(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets a creature ignore the “summoning sickness” rule
class Haste(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Precludes a permanent or player from being targeted by an opponent
class Hexproof(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Precludes a permanent from being destroyed
class Indestructible(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Causes a player to gain life
class Lifelink(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Makes creatures unblockable by a single creature
class Menace(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Causes the creature to get +1/+1 whenever its controller casts a noncreature spell
class Prowess(TriggeredAbility):
    def __init__(self, gameState, card):
        TriggeredAbility.__init__(self, gameState, card)

    def action(self):
        card.addPower(1)
        card.addToughness(1)

    def checkEvent(self):
        if (controller.cast()):
            return True
        
        return False


# Allows the creature to block an attacking creature with flying
class Reach(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Modifies how the creature assigns combat damage
class Trample(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets the creature attack without tapping
class Vigilance(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


#####################
# KEYWORD ABILITIES #
#####################

# Puts +1/+1 counters on the creature if there is none on it yet
class Adapt(ActivatedAbility):
    def __init__(self, gameState, card, counterValue, manaCost):
        ActivatedAbility.__init__(self, gameState, card)
        self.manaCost = manaCost

    def action(self):
        for i in range(counterValue):
            card.addCounter()

    def payCost(self):
        gameState.player.payMana(manaCost)


# Makes the defending player lose life for blocking
class Afflict(TriggeredAbility):
    def __init__(self, gameState, card):
        TriggeredAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Creates an amount of white and black Spirit creature tokens with flying when a creature dies
class Afterlife(TriggeredAbility):
    def __init__(self, gameState, card):
        TriggeredAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets a player cast one half of a Split card only from their graveyard
class Aftermath(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Causes a player to get the designation og the City's blessing once he controls 10 permanents
class Ascend(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets the player tap creatures rather than pay mana to cast a spell
class Convoke(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets the player tap creatures to turn a Vehicle into an artifact creature
class Crew(ActivatedAbility):
    def __init__(self, gameState, card):
        ActivatedAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets a card be discarded and be replaced with a new card
class Cycling(ActivatedAbility):
    def __init__(self, gameState, card):
        ActivatedAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets a player exile a creature card from their graveyard to create a mummified token version of that card
class Embalm(ActivatedAbility):
    def __init__(self, gameState, card):
        ActivatedAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets a player exile a creature card from their graveyard to create an eternalized token version of that card
class Eternalize(ActivatedAbility):
    def __init__(self, gameState, card):
        ActivatedAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets a player exile a creature card from their graveyard to create an eternalized token version of that card
class Exert():
    def __init__(self, gameState, card):
        .__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Causes a player to reveal the top card of their library and then to take different actions depending on whether a land card is revealed this way
class Explore():
    def __init__(self, gameState, card):
        .__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets the player choose whether to create Servo tokens or put +1/+1 counters on a creature when it enters the battlefield
class Fabricate(TriggeredAbility):
    def __init__(self, gameState, card):
        TriggeredAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets the player tap artifacts rather than pay mana to cast a spell
class Improvise(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets the player play Instants and Sorceries from your graveyard by discarding another card, paying mana and exiling the spell
class JumpStart(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Represents an optional additional cost. A spell has been kicked if its controller declared the intention to pay any or all of its kicker costs
class Kicker(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets a player put a +1/+1 counter on another attacking creature with less power when attacking with a creature with Mentor
class Mentor(TriggeredAbility):
    def __init__(self, gameState, card):
        TriggeredAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Lets a player chosse if a creature enters the battlefield with haste or a +1/+1 counter
class Riot(TriggeredAbility):
    def __init__(self, gameState, card):
        TriggeredAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Gives a spell an alternative mana cost and/or altered effect if an opponent lost life this turn
class Spectacle(StaticAbility):
    def __init__(self, gameState, card):
        StaticAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Look at the top cards of your library, put any number of them into your graveyard and the rest on top of your library
class Surveil():
    def __init__(self, gameState, card):
        .__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Allows to turn double-faced cards upside down
class Transform(TriggeredAbility): #Also activated?
    def __init__(self, gameState, card):
        TriggeredAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


#################
# ABILITY WORDS #
#################

# Improves or alters the effect of a spell if it is cast during a main phase
class Addendum(SpellAbility):
    def __init__(self, gameState, card):
        SpellAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Gives an advantageous effect when the creature receives damage
class Enrage(TriggeredAbility):
    def __init__(self, gameState, card):
        TriggeredAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Checks to see if you've attacked with a creature during that turn
class Raid(TriggeredAbility):
    def __init__(self, gameState, card):
        TriggeredAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# Checks whether a permanent you controlled left the battlefield this turn or not in order to alter a spell's effect
class Revolt(TriggeredAbility):
    def __init__(self, gameState, card):
        TriggeredAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


# 
class Undergrowth(TriggeredAbility):
    def __init__(self, gameState, card):
        TriggeredAbility.__init__(self, gameState, card)

    def action(self):
        #TODO
        pass


#################
# MAIN FUNCTION #
#################


# Main function
def main():
    if len(sys.argv) < 3:
        sys.exit(
            "Error: missing argument. Arguments are (in this order): gameState, card")

    Ability(sys.argv[1], sys.argv[2])


if (__name__ == "__main__"):
    main()

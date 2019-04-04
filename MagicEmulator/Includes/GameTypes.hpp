#ifndef GAMETYPES_HPP
#define GAMETYPES_HPP

enum CardType {

	Land = 0,
	Creature = 1,
	Artifact = 2,
	Enchantment= 3,
	Planeswalker = 4,
	Sorcery = 5,
	Instant = 6
};

enum Area {

	Library = 0,
	Hand = 1,
	Battlefield = 2,
	Graveyard = 3,
	Stack = 4,
	Exile = 5,
	Command = 6
};

enum Mana {

	White = 0,
	Blue = 1,
	Black = 2,
	Red = 3,
	Green = 4,
	Colorless = 5,
	AnyColor = 6
};

enum CurrentStep {

	UntapStep = 0,
	UpkeepStep = 1,
	DrawStep = 2,
	MainPhaseStep1 = 3,
	DeclaringAttackPhaseStep = 4,
	DeclaringAttackersStep = 5,
	DeclaringDefendersStep = 6,
	CombatDamagesStep = 7,
	EndCombatStep = 8,
	MainPhaseStep2 = 9,
	EndStep = 10,
	CleanupStep = 11
};

enum CurrentPhase {

	BeginningPhase = 0,
	MainPhase1 = 1,
	CombatPhase = 2,
	MainPhase2 = 3,
	EndPhase = 4
};

#endif
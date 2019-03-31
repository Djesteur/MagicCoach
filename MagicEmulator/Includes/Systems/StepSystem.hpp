#ifndef STEPSYSTEM_HPP
#define STEPSYSTEM_HPP

#include "Systems/System.hpp"

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


class StepSystem : public System {

	public:

		StepSystem(ComponentKeeper &keeper);

		void addEntity(const Entity currentEntity);

		int getCurrentTurn() const;

		int getCurrentStep() const;
		int getCurrentPhase() const;

		void nextStep();

	protected:

		Entity m_gameEntity;
};

#endif
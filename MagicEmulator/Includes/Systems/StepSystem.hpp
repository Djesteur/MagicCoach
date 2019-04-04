#ifndef STEPSYSTEM_HPP
#define STEPSYSTEM_HPP

#include "Systems/System.hpp"
#include "GameTypes.hpp"

class StepSystem : public System {

	public:

		StepSystem(ComponentKeeper &keeper, const Entity currentEntity);

		int getCurrentTurn() const;

		int getCurrentStep() const;
		int getCurrentPhase() const;

		int getActivePlayer() const;

		void nextStep();

	private:

		Entity m_gameEntity;
};

#endif
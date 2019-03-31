#include <iostream>
#include <memory>

#include "Components/IntegerComponent.hpp"
#include "ComponentKeeper.hpp"
#include "EntityCreator.hpp"
#include "Systems/StepSystem.hpp"

void drawInfos(StepSystem &testSystem) {

	std::cout << "Turn: " << testSystem.getCurrentTurn() 
			  << " - Phase: " << testSystem.getCurrentPhase()
			  << " - Step: " << testSystem.getCurrentStep() << std::endl;
}

int main() {

	EntityCreator creator;
	ComponentKeeper keeper;
	StepSystem testSystem{keeper};

	Entity gameEntity{creator.newEntity()}; 
	testSystem.addEntity(gameEntity);

	drawInfos(testSystem);
	testSystem.nextStep();
	drawInfos(testSystem);
	testSystem.nextStep();
	testSystem.nextStep();
	testSystem.nextStep();
	drawInfos(testSystem);
	testSystem.nextStep();
	testSystem.nextStep();
	drawInfos(testSystem);
	testSystem.nextStep();
	testSystem.nextStep();
	testSystem.nextStep();
	testSystem.nextStep();
	testSystem.nextStep();
	testSystem.nextStep();
	testSystem.nextStep();
	testSystem.nextStep();
	testSystem.nextStep();
	testSystem.nextStep();
	drawInfos(testSystem);

	return 0;
}
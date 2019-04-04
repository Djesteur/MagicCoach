#include <iostream>
#include <memory>

#include "Components/IntegerComponent.hpp"
#include "ComponentKeeper.hpp"
#include "EntityCreator.hpp"
#include "Systems/GameSystem.hpp"

int main() {

	EntityCreator creator;
	ComponentKeeper keeper;
	GameSystem testSystem{keeper, creator};

	testSystem.playGame();

	return 0;
}
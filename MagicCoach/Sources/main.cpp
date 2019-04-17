#include "Components/IntegerComponent.hpp"
#include "ComponentKeeper.hpp"
#include "EntityCreator.hpp"
#include "Systems/GameSystem.hpp"

int main() {

	EntityCreator creator;
	ComponentKeeper keeper;
	GameSystem gameSystem{keeper, creator};

	gameSystem.playGame();

	return 0;
}
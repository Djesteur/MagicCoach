#include <iostream>
#include <memory>

#include "Components/IntegerComponent.hpp"
#include "ComponentKeeper.hpp"
#include "EntityCreator.hpp"

int main() {

	EntityCreator creator;
	ComponentKeeper keeper;

	Entity e1{creator.newEntity()}, e2{creator.newEntity()};

	std::cout << e1 << " " << e2 << std::endl;

	std::shared_ptr<Component> compo1{std::make_shared<IntegerComponent>(ComponentType::Integer, 102)};

	keeper.addEntity(e1);
	keeper.addComponent(e1, "Test", compo1);

	std::shared_ptr<IntegerComponent> casted{std::static_pointer_cast<IntegerComponent>(compo1)};

	int& test1{std::static_pointer_cast<IntegerComponent>(compo1)->data()};

	std::cout << test1 << std::endl;
	std::cout << casted->data() << std::endl;
	std::cout << std::dynamic_pointer_cast<IntegerComponent>(compo1)->data() << std::endl;
	std::cout << std::dynamic_pointer_cast<IntegerComponent>(keeper.getComponent(e1, "Test"))->data() << std::endl;

	test1 = 3;

	std::cout << test1 << std::endl;
	std::cout << casted->data() << std::endl;
	std::cout << std::dynamic_pointer_cast<IntegerComponent>(compo1)->data() << std::endl;
	std::cout << std::dynamic_pointer_cast<IntegerComponent>(keeper.getComponent(e1, "Test"))->data() << std::endl;	
	
	return 0;
}
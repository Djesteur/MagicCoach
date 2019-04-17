#include "Components/ComponentUtils.hpp"

std::shared_ptr<Component> newComponent(const ComponentType newComponentType) {

	std::shared_ptr<Component> result{nullptr};

	switch(newComponentType) {

		case ComponentType::Boolean:
			result = std::make_shared<BooleanComponent>(newComponentType, false);
			break;


		case ComponentType::Integer:
			result = std::make_shared<IntegerComponent>(newComponentType, 0);
			break;

		case ComponentType::IntegerArray:
			result = std::make_shared<IntegerArrayComponent>(newComponentType);
			break;

		case ComponentType::Word:
			result = std::make_shared<WordComponent>(newComponentType, "");
			break;

		case ComponentType::WordArray:
			result = std::make_shared<WordArrayComponent>(newComponentType);
			break;

		case ComponentType::Target:
			result = std::make_shared<TargetComponent>(newComponentType, 0);
			break;

		case ComponentType::TargetArray:
			result = std::make_shared<TargetArrayComponent>(newComponentType);
			break;


		case ComponentType::Script:
			result = std::make_shared<ScriptComponent>(newComponentType, Script{});
			break;

		case ComponentType::Unknown:
			std::cout << "WARNING: asked to add an unkown component type, will return nullptr." << std::endl;
			break;

	}

	return result;
}
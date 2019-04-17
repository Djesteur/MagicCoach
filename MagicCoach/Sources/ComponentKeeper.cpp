#include "ComponentKeeper.hpp"

ComponentKeeper::ComponentKeeper() {}

void ComponentKeeper::addEntity(const Entity newEntity) {

	if(!existingEntity(newEntity)) { m_components.insert(std::make_pair(newEntity, std::map<std::string, std::shared_ptr<Component>>{})); }
}

void ComponentKeeper::deleteEntity(const Entity currentEntity) {

	if(existingEntity(currentEntity)) { m_components.erase(m_components.find(currentEntity)); }
}

bool ComponentKeeper::existingEntity(const Entity currentEntity) const {

	if(m_components.find(currentEntity) == m_components.end()) { return false; }
	return true;
}

void ComponentKeeper::addComponent(const Entity currentEntity, const std::string componentName, const std::shared_ptr<Component> &newComponent) {

	if(!existingEntity(currentEntity)) { m_components.insert(std::make_pair(currentEntity, std::map<std::string, std::shared_ptr<Component>>{})); }

	if(entityHasComponent(currentEntity, componentName)) { m_components[currentEntity][componentName] = newComponent; }
	else { m_components[currentEntity].insert(std::make_pair(componentName, newComponent)); }
}

void ComponentKeeper::deleteComponent(const Entity currentEntity, const std::string componentName) {

	if(entityHasComponent(currentEntity, componentName)) { //Also check if entity is existing

		m_components[currentEntity].erase(m_components[currentEntity].find(componentName));
	}
}
    
void ComponentKeeper::changeComponent(const Entity currentEntity, const std::string componentName, std::shared_ptr<Component> newComponent) {

	if(entityHasComponent(currentEntity, componentName)) { //Also check if entity is existing

		m_components[currentEntity][componentName] = newComponent;
	}
}

std::shared_ptr<Component> ComponentKeeper::getComponent(const Entity currentEntity, const std::string componentName) {

	if(entityHasComponent(currentEntity, componentName)) { return m_components[currentEntity][componentName]; }
	else {

		throw std::runtime_error("ComponentKeeper error: asked for component " + componentName + " of entity " 
								 + std::to_string(currentEntity) + ", which doesn't exist.");
	}
	return nullptr;
}

bool ComponentKeeper::entityHasComponent(const Entity currentEntity, const std::string componentName) {

	if(existingEntity(currentEntity)) {

		if(m_components[currentEntity].find(componentName) != m_components[currentEntity].end()) { return true; }
	}

	return false;
}

void ComponentKeeper::drawEntity(const Entity &entity) {

	if(existingEntity(entity)) {

			std::cout << "Entity: " << entity << std::endl;

		for(std::map<std::string, std::shared_ptr<Component>>::iterator componentIte = m_components[entity].begin();
			componentIte != m_components[entity].end(); componentIte++) {

			switch(componentIte->second->getType()) {

				case ComponentType::Boolean:
					std::cout << " Type: Boolean - Name: " << componentIte->first << " - Value: "
					<< std::static_pointer_cast<BooleanComponent>(componentIte->second)->data() << std::endl;
					break;

				case ComponentType::Integer:
					std::cout << " Type: Integer - Name: " << componentIte->first << " - Value: "
					<< std::static_pointer_cast<IntegerComponent>(componentIte->second)->data() << std::endl;
					break;

				case ComponentType::Target:
					std::cout << " Type: Target - Name: " << componentIte->first << " - Value: "
					<< std::static_pointer_cast<TargetComponent>(componentIte->second)->data() << std::endl;
					break;

				case ComponentType::Word:
					std::cout << " Type: Word - Name: " << componentIte->first << " - Value: "
					<< std::static_pointer_cast<WordComponent>(componentIte->second)->data() << std::endl;
					break;

				case ComponentType::IntegerArray:
					std::cout << " Type: Word - Name: " << componentIte->first << " - Value: ";
					for(int &currentValue: std::static_pointer_cast<IntegerArrayComponent>(componentIte->second)->data()) { 
						
						std::cout << currentValue << " "; 
					} 
					std::cout << std::endl;
					break;

				default:
					break;
			}
		}
	}
}

void ComponentKeeper::drawComponents() {

	for(std::map<Entity, std::map<std::string, std::shared_ptr<Component>>>::iterator entityIte = m_components.begin();
		entityIte != m_components.end(); entityIte++) {

		drawEntity(entityIte->first);
	}
}

/*enum class ComponentType {

	Boolean,
	Integer,
	IntegerArray,
	Word,
	WordArray,
	Target,
	TargetArray,
	Script,
	Unknown

};*/
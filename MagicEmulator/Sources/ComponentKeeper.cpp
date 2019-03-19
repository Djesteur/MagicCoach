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
	return nullptr;
}

bool ComponentKeeper::entityHasComponent(const Entity currentEntity, const std::string componentName) {

	if(existingEntity(currentEntity)) {

		if(m_components[currentEntity].find(componentName) != m_components[currentEntity].end()) { return true; }
	}

	return false;
}
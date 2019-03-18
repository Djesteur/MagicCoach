#include "ComponentKeeper.hpp"

ComponentKeeper::ComponentKeeper() {}

void ComponentKeeper::addEntity(const Entity newEntity) {

	if(!existingEntity(newEntity)) { m_components.insert(std::make_pair(newEntity, std::vector<ComponentHolder>{})); }
}

void ComponentKeeper::deleteEntity(const Entity currentEntity) {

	if(existingEntity(currentEntity)) { m_components.erase(m_components.find(currentEntity)); }
}

bool ComponentKeeper::existingEntity(const Entity currentEntity) const {

	if(m_components.find(currentEntity) == m_components.end()) { return false; }
	return true;
}

void ComponentKeeper::addComponentToEntity(const Entity currentEntity, const std::string componentName, const Component &newComponent) {


}

void ComponentKeeper::deleteComponentToEntity(const Entity currentEntity, const std::string componentName) {}

void ComponentKeeper::changeComponentOfEntity(const Entity currentEntity, const std::string componentName, std::shared_ptr<Component> newComponent) {}
std::shared_ptr<Component> ComponentKeeper::getComponentOfEntity(const Entity currentEntity, const std::string componentName) {}

bool entityHasComponent
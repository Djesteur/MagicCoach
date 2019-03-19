#include "Systems/System.hpp"

System::System() {}

void System::deleteEntity(const Entity &currentEntity) {

	std::vector<Entity>::iterator it{std::find(m_entities.begin(), m_entities.end(), currentEntity)};

	if(it != m_entities.end()) { m_entities.erase(it); }
}

bool System::isInSystem(const Entity &currentEntity) const {

	if(std::find(m_entities.begin(), m_entities.end(), currentEntity) != m_entities.end()) { return true; }
	return false;
}
#include "EntityCreator.hpp"

//Let 0 can be used as an invalid entity

EntityCreator::EntityCreator(): m_lastEntity{1} {}

Entity EntityCreator::newEntity() {

	m_lastEntity++;
	return m_lastEntity;
}
#include "EntityCreator.hpp"

EntityCreator::EntityCreator(): m_lastEntity{0} {}

Entity EntityCreator::newEntity() {

	m_lastEntity++;
	return m_lastEntity;
}
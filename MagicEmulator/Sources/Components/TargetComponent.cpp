#include "Components/TargetComponent.hpp"

TargetComponent::TargetComponent(ComponentType type, Entity value): 
	Component{type},
	m_value{value} {}

TargetComponent::~TargetComponent() {}

Entity& TargetComponent::data() { return m_value; }
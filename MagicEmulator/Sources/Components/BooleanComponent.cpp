#include "Components/BooleanComponent.hpp"

BooleanComponent::BooleanComponent(ComponentType type, bool value):
	Component{type},
	m_value{value} {}

bool& BooleanComponent::getData() { return m_value; }
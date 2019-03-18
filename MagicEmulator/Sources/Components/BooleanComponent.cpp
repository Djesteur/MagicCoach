#include "Components/BooleanComponent.hpp"

BooleanComponent::BooleanComponent(ComponentType type, bool value):
	Component{type},
	m_value{value} {}

bool& BooleanComponent::data() { return m_value; }
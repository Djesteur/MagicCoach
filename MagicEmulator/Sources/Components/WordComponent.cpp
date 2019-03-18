#include "Components/WordComponent.hpp"

WordComponent::WordComponent(ComponentType type, std::string value):
	Component{type},
	m_value{value} {}

std::string& WordComponent::data() { return m_value; }
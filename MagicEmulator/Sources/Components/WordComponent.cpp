#include "Components/WordComponent.hpp"

WordComponent::WordComponent(ComponentType type, std::string value):
	Component{type},
	m_value{value} {}

WordComponent::~WordComponent() {}

std::string& WordComponent::data() { return m_value; }
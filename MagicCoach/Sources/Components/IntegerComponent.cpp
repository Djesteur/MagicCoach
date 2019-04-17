#include "Components/IntegerComponent.hpp"

IntegerComponent::IntegerComponent(ComponentType type, int value):
	Component{type},
	m_value{value} {}

IntegerComponent::~IntegerComponent() {}

int& IntegerComponent::data() { return m_value; }
#include "Components/IntegerArrayComponent.hpp"

IntegerArrayComponent::IntegerArrayComponent(ComponentType type): Component{type} {}

IntegerArrayComponent::~IntegerArrayComponent() {}

std::vector<int>& IntegerArrayComponent::data() { return m_value; }
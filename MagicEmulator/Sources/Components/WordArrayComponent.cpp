#include "Components/WordArrayComponent.hpp"

WordArrayComponent::WordArrayComponent(ComponentType type): Component{type} {}

std::vector<std::string>& WordArrayComponent::data() { return m_value; }
#include "Components/TargetArrayComponent.hpp"

TargetArrayComponent::TargetArrayComponent(ComponentType type): Component{type} {}

std::vector<Entity>& TargetArrayComponent::data() { return m_value; }
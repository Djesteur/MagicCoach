#include "Components/TargetArrayComponent.hpp"

TargetArrayComponent::TargetArrayComponent(ComponentType type): Component{type} {}

TargetArrayComponent::~TargetArrayComponent() {}

std::vector<Entity>& TargetArrayComponent::data() { return m_value; }
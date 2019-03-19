#include "Components/ScriptComponent.hpp"

ScriptComponent::ScriptComponent(ComponentType type, Script value):
	Component{type},
	m_value{value} {}

ScriptComponent::~ScriptComponent() {}

Script& ScriptComponent::data() { return m_value; }
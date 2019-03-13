#include "Components/Component.hpp"

Component::Component(ComponentType type): m_type{type} {}

ComponentType Component::getType() const { return m_type; }

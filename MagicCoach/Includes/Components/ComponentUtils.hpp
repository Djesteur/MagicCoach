#ifndef COMPONENTUTILS_HPP
#define COMPONENTUTILS_HPP

#include <memory>
#include <iostream>
#include <string>

#include "Components/BooleanComponent.hpp"
#include "Components/IntegerComponent.hpp"
#include "Components/IntegerArrayComponent.hpp"
#include "Components/ScriptComponent.hpp"
#include "Components/TargetComponent.hpp"
#include "Components/TargetArrayComponent.hpp"
#include "Components/WordComponent.hpp"
#include "Components/WordArrayComponent.hpp"

std::shared_ptr<Component> newComponent(const ComponentType newComponentType);

#endif
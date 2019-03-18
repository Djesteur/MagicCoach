#ifndef TARGETCOMPONENT_HPP
#define TARGETCOMPONENT_HPP

#include <cstdint>

#include "Components/Component.hpp"

using Entity = uint32_t;

class TargetComponent: public Component {

	public:

		TargetComponent(ComponentType type, Entity value);

		Entity& data();

	private:

		Entity m_value;
};

#endif
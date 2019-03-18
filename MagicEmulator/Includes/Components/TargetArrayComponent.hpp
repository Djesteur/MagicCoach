#ifndef TARGETARRAYCOMPONENT_HPP
#define TARGETARRAYCOMPONENT_HPP

#include <vector>
#include <cstdint>

#include "Components/Component.hpp"

using Entity = uint32_t;

class TargetArrayComponent: public Component {

	public:

		TargetArrayComponent(ComponentType type);

		std::vector<Entity>& data();

	private:

		std::vector<Entity> m_value;
};

#endif
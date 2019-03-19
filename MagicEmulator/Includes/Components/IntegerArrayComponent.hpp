#ifndef INTEGERARRAYCOMPONENT_HPP
#define INTEGERARRAYCOMPONENT_HPP

#include <vector>

#include "Components/Component.hpp"

class IntegerArrayComponent: public Component {

	public:

		IntegerArrayComponent(ComponentType type);
		virtual ~IntegerArrayComponent();

		std::vector<int>& data();

	private:

		std::vector<int> m_value;
};

#endif
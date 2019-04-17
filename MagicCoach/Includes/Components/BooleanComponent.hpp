#ifndef BOOLEANCOMPONENT_HPP
#define BOOLEANCOMPONENT_HPP

#include "Components/Component.hpp"

class BooleanComponent: public Component {

	public:

		BooleanComponent(ComponentType type, bool value);
		virtual ~BooleanComponent();

		bool& data();

	private:

		bool m_value;
};

#endif
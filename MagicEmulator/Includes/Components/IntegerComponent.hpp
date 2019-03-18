#ifndef INTEGERCOMPONENT_HPP
#define INTEGERCOMPONENT_HPP

#include "Components/Component.hpp"

class IntegerComponent: public Component {

	public:

		IntegerComponent(ComponentType type, int value);

		int& data();

	private:

		int m_value;
};

#endif
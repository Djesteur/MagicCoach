#ifndef SCRIPTCOMPONENT_HPP
#define SCRIPTCOMPONENT_HPP

#include "Components/Component.hpp"

struct Script {};

class ScriptComponent: public Component {

	public:

		ScriptComponent(ComponentType type, Script value);

		Script& data();

	private:

		Script m_value;
};

#endif
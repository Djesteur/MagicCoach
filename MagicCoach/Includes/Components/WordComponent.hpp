#ifndef WORDCOMPONENT_HPP
#define WORDCOMPONENT_HPP

#include <string>

#include "Components/Component.hpp"

class WordComponent: public Component {

	public:

		WordComponent(ComponentType type, std::string value);
		virtual ~WordComponent();

		std::string& data();

	private:

		std::string m_value;
};

#endif
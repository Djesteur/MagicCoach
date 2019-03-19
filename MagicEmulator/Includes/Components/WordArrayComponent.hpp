#ifndef WORDARRAYCOMPONENT_HPP
#define WORDARRAYCOMPONENT_HPP

#include <vector>
#include <string>

#include "Components/Component.hpp"

class WordArrayComponent: public Component {

	public:

		WordArrayComponent(ComponentType type);
		virtual ~WordArrayComponent();
		
		std::vector<std::string>& data();

	private:

		std::vector<std::string> m_value;
};

#endif
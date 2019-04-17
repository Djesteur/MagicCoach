#ifndef IA_HPP
#define IA_HPP

#include "EntityCreator.hpp"

class IA {

	public:

		IA(const Entity player);

		void rien();
	private:

		Entity m_player;
};

#endif
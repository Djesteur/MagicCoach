#ifndef CARDLOADER_HPP
#define CARDLOADER_HPP

#include <map>

#include "ComponentKeeper.hpp"
#include "EntityCreator.hpp"

class CardLoader {

	public:

		CardLoader(ComponentKeeper &keeper, EntityCreator &creator);
		CardLoader(const CardLoader&) = delete;
		CardLoader &operator=(const CardLoader&) = delete;

		Entity getCard(const unsigned int id);
		//Make a copy of loaded card, and return it

	private:

		Entity loadFromFile(const std::string path, const unsigned int id);

		ComponentKeeper &m_keeper;
		EntityCreator &m_creator; 
		std::map<unsigned int, Entity> m_loadedCard;
};

#endif
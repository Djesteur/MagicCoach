#include "CardLoader.hpp"

CardLoader::CardLoader(const ComponentKeeper &keeper, const EntityCreator &creator) :
	m_keeper{ keeper },
	m_creator{ creator } {}

Entity CardLoader::getCard(const unsigned int id) {

	if(m_loadedCard.find(id) != m_loadedCard.end()) { return m_loadedCard[id]; }
	else { m_loadedCard.insert(std::make_pair(id, loadFromFile("Le chemin du fichier", id)); }

	return m_loadedCard[id];
}

Entity CardLoader::loadFromFile(const std::string path, const unsigned int id) {

	Entity newCard{ m_creator.newEntity() };


	return newCard;
}
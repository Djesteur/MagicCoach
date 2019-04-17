#ifndef MANASYSTEM_HPP
#define MANASYSTEM_HPP

#include <deque>
#include <array>
#include <string>

#include "Systems/System.hpp"
#include "GameTypes.hpp"

class ManaSystem : public System {

	public:

		ManaSystem(ComponentKeeper &keeper, const Entity currentEntity, EntityCreator &creator);

		//Return true if can was played, false if not enought mana in mana pool
		bool playCard(const unsigned int player, const Entity &cardToPlay);

		void untapStep(const unsigned int player);
		void endOfStep();

		void getManaFromLand(const Entity &landEntity);
		void tapLandForMana(const unsigned int player, const Entity &land);

		
		std::vector<int> listAllPossibleMana(const unsigned int player) const;
		int getManaFromName(const std::string name) const;

	private:

		Entity m_gameEntity;
		EntityCreator &m_entityCreator;

		std::array<std::deque<Entity>, 2> m_playerLand;

		std::array<Entity, 2> m_manaPool;

		bool m_landPlayedThisTurn;
};

#endif
#ifndef GAMESYSTEM_HPP
#define GAMESYSTEM_HPP

#include <deque>
#include <algorithm>
#include <stack>
#include <ctime>
#include <cstdlib>

#include "Systems/System.hpp"
#include "Systems/StepSystem.hpp"
#include "Systems/StackSystem.hpp"
#include "Systems/ManaSystem.hpp"
#include "EntityCreator.hpp"

#include "GameTypes.hpp"

class GameSystem : public System {

	public:

		GameSystem(ComponentKeeper &keeper, EntityCreator &creator);

		void playGame();

	private:

		void drawXCards(const unsigned int player, const unsigned int x);
		void drawCard(const unsigned int player);

		void prepareBrandonGame();
		void iaAttackAll(const unsigned int activePlayer);
		void iaPlayAll(const unsigned int activePlayer);
		void drawBoard(const unsigned int activePlayer);

		EntityCreator &m_creator;
		Entity m_gameEntity;

		std::array<std::deque<Entity>, 2> m_playerCards, 
										  m_library, 
										  m_hand, 
										  m_battlefield,
										  m_graveyard,
										  m_exile,
										  m_command;

		StepSystem m_stepSystem;
		StackSystem m_stackSystem;
		ManaSystem m_manaSystem;
};

#endif
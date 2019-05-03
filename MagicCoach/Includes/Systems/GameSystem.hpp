#ifndef GAMESYSTEM_HPP
#define GAMESYSTEM_HPP

#include <deque>
#include <algorithm>
#include <stack>
#include <ctime>
#include <cstdlib>

#include "EntityCreator.hpp"
#include "GameTypes.hpp"

#include "Systems/System.hpp"
#include "Systems/StepSystem.hpp"
#include "Systems/StackSystem.hpp"
#include "Systems/ManaSystem.hpp"
#include "Systems/StateBasedActionSystem.hpp"
#include "Systems/AttackSystem.hpp"

#include "Transmitter.hpp"


class GameSystem : public System {

	public:

		GameSystem(ComponentKeeper &keeper, EntityCreator &creator, Transmitter &transmitter);

		void playGame();

	private:

		void drawXCards(const unsigned int player, const unsigned int x);
		void drawCard(const unsigned int player);

		void prepareBrandonGame();
		void iaAttackAll(const unsigned int activePlayer);
		void iaPlayAll(const unsigned int activePlayer);
		void drawBoard(const unsigned int activePlayer);
		void resetDamageTaken();
		void removeSummonSickness(const unsigned int activePlayer);

		EntityCreator &m_creator;
		Entity m_gameEntity;

		Entity m_player1, m_player2;

		std::array<std::deque<Entity>, 2> m_playerCards, m_library;

		StepSystem m_stepSystem;
		StackSystem m_stackSystem;
		ManaSystem m_manaSystem;
		StateBasedActionSystem m_actionSystem;
		AttackSystem m_attackSystem;

		Transmitter &m_transmitter;
};

#endif
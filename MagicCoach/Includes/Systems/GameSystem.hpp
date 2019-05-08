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
#include "CardLoader.hpp"

struct WaitingMtgaID {

	WaitingMtgaID::WaitingMtgaID(const int inst, const int play):
		instanceID{inst},
		player{play} {}

	int instanceID, player;
};

class GameSystem : public System {

	public:

		GameSystem(ComponentKeeper &keeper, EntityCreator &creator, Transmitter &transmitter);

		void playGame();

	private:

		void receiveStepInfo(const Information lastInfo);

		void drawBoard(const unsigned int activePlayer);
		void resetDamageTaken();
		void removeSummonSickness(const unsigned int activePlayer);

		bool canBePlayed(const int instanceID);
		void playCard(const int instanceID, const int player);
		void addToWaitingID(const int instanceID, const int player);
		void checkMtgaID(const int instanceID, const int mtgaID);
		void checkCardsToPlay();

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

		CardLoader m_cardLoader;

		std::map<int, int> m_correspondenceInstanceMtga;
		std::queue<WaitingMtgaID> m_idWaitingMtga;
};

#endif
#include "Systems/GameSystem.hpp"

GameSystem::GameSystem(ComponentKeeper &keeper, EntityCreator &creator): 
	System{keeper},
	m_creator{creator},
	m_gameEntity{creator.newEntity()},
	m_stepSystem{keeper, m_gameEntity},
	m_stackSystem{keeper, m_gameEntity},
	m_manaSystem{keeper, m_gameEntity, creator} {

	std::srand(std::time(0)); //Init random for std::random_shuffle
	prepareBrandonGame();

	std::cout << "Begin of the game, players draw 7 cards. " << std::endl;
	drawXCards(0, 7);
	drawXCards(1, 7);

	//std::cout << "Players have: " << std::endl;

	/*for(unsigned int i{0}; i < m_hand[0].size(); i++) { m_keeper.drawEntity(m_hand[0][i]); }
	std::cout << "-----------------------------" << std::endl;
	
	for(unsigned int i{0}; i < m_hand[1].size(); i++) { m_keeper.drawEntity(m_hand[1][i]); }
	std::cout << "-----------------------------" << std::endl;*/

	/*while(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_gameEntity, "NumberOfTurn"))->data() < 5 ) {

		m_stepSystem.nextStep();

		if(m_stepSystem.getCurrentStep() == CurrentStep::DrawStep) {

			drawCard(m_stepSystem.getActivePlayer());
			std::cout << "Player " << m_stepSystem.getActivePlayer() << " draw the following card: " << std::endl;
			m_keeper.drawEntity(m_hand[m_stepSystem.getActivePlayer()].back());
		}
	}*/
}

void GameSystem::playGame() {

	unsigned int test{0};

	int currentStep{0}, activePlayer{0};

	while(test < 100) {

		currentStep = std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_gameEntity, "CurrentStep"))->data();
		activePlayer = std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_gameEntity, "ActivePlayer"))->data();

		switch(currentStep) {

			case CurrentStep::UntapStep:
				m_manaSystem.untapStep(activePlayer);
				break;

			case CurrentStep::UpkeepStep:
				break;

			case CurrentStep::DrawStep:
				drawCard(activePlayer);
				break;

			case CurrentStep::MainPhaseStep1:
				break;

			case CurrentStep::DeclaringAttackPhaseStep:
				break;

			case CurrentStep::DeclaringAttackersStep:
				iaAttackAll(activePlayer);
				break;

			case CurrentStep::DeclaringDefendersStep:
				break;

			case CurrentStep::CombatDamagesStep:
				
				break;

			case CurrentStep::EndCombatStep:
				break;

			case CurrentStep::MainPhaseStep2:
				iaPlayAll(activePlayer);
				break;

			case CurrentStep::EndStep:
				drawBoard(activePlayer);
				break;

			case CurrentStep::CleanupStep:
				//Discard
				//Reset blessures
				break;

		}

		m_manaSystem.endOfStep();
		m_stepSystem.nextStep();

		test++;
	}
}

void GameSystem::drawXCards(const unsigned int player, const unsigned int x) {

	for(unsigned int i{0}; i < x; i++) { drawCard(player); }
}

void GameSystem::drawCard(const unsigned int player) {

	Entity movingCard{m_library[player].front()};
	m_library[player].pop_front();
	m_hand[player].emplace_back(movingCard);

	std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_hand[player].back(), "Area"))->data() = Area::Hand;
}

void GameSystem::prepareBrandonGame() {

	Entity newCard;

	//Add brandons
	for(unsigned int i{0}; i < 72; i++) {

		newCard = m_creator.newEntity();
		m_keeper.addEntity(newCard);

		std::shared_ptr<Component> cardName{newComponent(ComponentType::Word)};
		std::static_pointer_cast<WordComponent>(cardName)->data() = "Brandon de la flotte des grands fonds";
		m_keeper.addComponent(newCard, "Name", cardName);

		std::shared_ptr<Component> cardArea{newComponent(ComponentType::Integer)};
		std::static_pointer_cast<IntegerComponent>(cardArea)->data() = Area::Library;
		m_keeper.addComponent(newCard, "Area", cardArea);

		std::shared_ptr<Component> cardType{newComponent(ComponentType::Integer)};
		std::static_pointer_cast<IntegerComponent>(cardType)->data() = CardType::Creature;
		m_keeper.addComponent(newCard, "Type", cardType);

		std::shared_ptr<Component> cardManaCost{newComponent(ComponentType::IntegerArray)};
		std::static_pointer_cast<IntegerArrayComponent>(cardManaCost)->data().emplace_back(Mana::AnyColor);
		std::static_pointer_cast<IntegerArrayComponent>(cardManaCost)->data().emplace_back(Mana::Red);
		m_keeper.addComponent(newCard, "ManaCost", cardManaCost);

		std::shared_ptr<Component> cardCMC{newComponent(ComponentType::Integer)};
		std::static_pointer_cast<IntegerComponent>(cardCMC)->data() = 2;
		m_keeper.addComponent(newCard, "CMC", cardCMC);

		std::shared_ptr<Component> cardController{newComponent(ComponentType::Integer)};
		m_keeper.addComponent(newCard, "Controller", cardController);

		if(i < 36) { 

			m_playerCards[0].emplace_back(newCard);
			std::static_pointer_cast<IntegerComponent>(cardController)->data() = 0;
		}

		else { 

			m_playerCards[1].emplace_back(newCard);
			std::static_pointer_cast<IntegerComponent>(cardController)->data() = 1;
		}

		std::shared_ptr<Component> cardTapped{newComponent(ComponentType::Boolean)};
		std::static_pointer_cast<BooleanComponent>(cardTapped)->data() = false;
		m_keeper.addComponent(newCard, "IsTapped", cardTapped);

		std::shared_ptr<Component> cardSummon{newComponent(ComponentType::Boolean)};
		std::static_pointer_cast<BooleanComponent>(cardSummon)->data() = true;
		m_keeper.addComponent(newCard, "SummonSickness", cardSummon);
	}

	//Add lands

	for(unsigned int i{0}; i < 48; i++) {

		newCard = m_creator.newEntity();
		m_keeper.addEntity(newCard);
		
		if(i < 36) { m_playerCards[0].emplace_back(newCard); }
		else { m_playerCards[1].emplace_back(newCard); }

		std::shared_ptr<Component> cardName{newComponent(ComponentType::Word)};
		std::static_pointer_cast<WordComponent>(cardName)->data() = "Mountain";
		m_keeper.addComponent(newCard, "Name", cardName);

		std::shared_ptr<Component> cardArea{newComponent(ComponentType::Integer)};
		std::static_pointer_cast<IntegerComponent>(cardArea)->data() = Area::Library;
		m_keeper.addComponent(newCard, "Area", cardArea);

		std::shared_ptr<Component> cardType{newComponent(ComponentType::Integer)};
		std::static_pointer_cast<IntegerComponent>(cardType)->data() = CardType::Land;
		m_keeper.addComponent(newCard, "Type", cardType);

		std::shared_ptr<Component> cardCMC{newComponent(ComponentType::Integer)};
		std::static_pointer_cast<IntegerComponent>(cardCMC)->data() = 0;
		m_keeper.addComponent(newCard, "CMC", cardCMC);

		std::shared_ptr<Component> cardController{newComponent(ComponentType::Integer)};
		m_keeper.addComponent(newCard, "Controller", cardController);

		if(i < 36) { 

			m_playerCards[0].emplace_back(newCard);
			std::static_pointer_cast<IntegerComponent>(cardController)->data() = 0;
		}

		else { 

			m_playerCards[1].emplace_back(newCard);
			std::static_pointer_cast<IntegerComponent>(cardController)->data() = 1;
		}

		std::shared_ptr<Component> cardTapped{newComponent(ComponentType::Boolean)};
		std::static_pointer_cast<BooleanComponent>(cardTapped)->data() = false;
		m_keeper.addComponent(newCard, "IsTapped", cardTapped);
	}

	//Sort library

	m_library[0] = m_playerCards[0];
	m_library[1] = m_playerCards[1];

	std::random_shuffle(m_library[0].begin(), m_library[0].end());
	std::random_shuffle(m_library[1].begin(), m_library[1].end());
}

void GameSystem::iaAttackAll(const unsigned int activePlayer) {


}

void GameSystem::iaPlayAll(const unsigned int activePlayer) {

	//Play a montain if have 1

	for(unsigned int i{0}; i < m_hand[activePlayer].size(); i++) {

		if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_hand[activePlayer][i], "Type"))->data() == CardType::Land) {

			if(m_manaSystem.canPlay(activePlayer, m_hand[activePlayer][i])) { 

				m_manaSystem.playCard(activePlayer, m_hand[activePlayer][i]);
				//Card put on board by mana system

				m_battlefield[activePlayer].emplace_back(m_hand[activePlayer][i]);
				m_hand[activePlayer].erase(m_hand[activePlayer].begin() + i);
			}
		}
	}

	//Play the most possible number of brandon

	for(unsigned int i{0}; i < m_hand[activePlayer].size(); i++) {

		if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_hand[activePlayer][i], "Type"))->data() == CardType::Creature) {

			if(m_manaSystem.canPlay(activePlayer, m_hand[activePlayer][i])) { 

				//Tap land until get enought mana (try to get mana from all cards, mana system willtap only lands)
				bool cardPlayed{false};
				for(unsigned int j{0}; j < m_battlefield[activePlayer].size() && !cardPlayed; j++) {

					m_manaSystem.tapLandForMana(activePlayer, m_battlefield[activePlayer][j]);
					cardPlayed = m_manaSystem.playCard(activePlayer, m_hand[activePlayer][i]);
				}
				
				m_stackSystem.wantToPlay(m_hand[activePlayer][i]);
				m_stackSystem.resolveAllSpell();

				m_battlefield[activePlayer].emplace_back(m_hand[activePlayer][i]);
				m_hand[activePlayer].erase(m_hand[activePlayer].begin() + i);

				i = 0;
			}
		}
	}
}

void GameSystem::drawBoard(const unsigned int activePlayer) {

	std::cout << "Player " << activePlayer << " has on hand: " << std::endl;

	for(unsigned int i{0}; i < m_hand[activePlayer].size(); i++) {

		std::cout << "  " << std::static_pointer_cast<WordComponent>(m_keeper.getComponent(m_hand[activePlayer][i], "Name"))->data() << std::endl;
	}

	std::cout << "Player " << activePlayer << " has on board: " << std::endl;

	for(unsigned int i{0}; i < m_battlefield[activePlayer].size(); i++) {

		std::cout << "  " <<std::static_pointer_cast<WordComponent>(m_keeper.getComponent(m_battlefield[activePlayer][i], "Name"))->data() << std::endl;
	}
	std::cout << "---------------------------------------------------------------" << std::endl;
}
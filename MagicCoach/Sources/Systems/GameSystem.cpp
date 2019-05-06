#include "Systems/GameSystem.hpp"

GameSystem::GameSystem(ComponentKeeper &keeper, EntityCreator &creator, Transmitter &transmitter): 
	System{keeper},
	m_creator{creator},
	m_gameEntity{creator.newEntity()},
	m_player1{m_creator.newEntity()},
	m_player2{m_creator.newEntity()},
	m_stepSystem{keeper, m_gameEntity},
	m_stackSystem{keeper, m_gameEntity},
	m_manaSystem{keeper, m_gameEntity, creator},
	m_actionSystem{keeper, m_gameEntity, m_player1, m_player2},
	m_attackSystem{keeper, m_player1, m_player2},
	m_transmitter{transmitter} {

	std::cout << "Emulator is ready !" << std::endl;
}

void GameSystem::playGame() {

	std::cout << "Begin of the game." << std::endl;

	bool player1Lost{false}, player2Lost{false}, haveToQuit{false};
	unsigned int activePlayer{0};
	int currentStep{0};

	while(!haveToQuit) {

		activePlayer = std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_gameEntity, "ActivePlayer"))->data();

		//TODO: une fonction qui resynchronise vraiment selon les données reçues

		if(m_transmitter.waitingInfoForCoach()) {

			Information lastInfo{m_transmitter.getCoachInfo()};

			if(lastInfo.type == InformationType::NextStep) {

				m_stepSystem.nextStep();
				currentStep = m_stepSystem.getCurrentStep();

				if(currentStep != lastInfo.values[0]) {

					std::cout  << "Emulator was desynchronized (step " << currentStep << " instead of step " << lastInfo.values[0] << ")." << std::endl;
					std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_gameEntity, "CurrentStep"))->data() = lastInfo.values[0];
					//TODO: resynchoniser également les phases (pas important)
				}

				std::cout << "Begin of step " << currentStep << std::endl;

				switch(currentStep) {

					case CurrentStep::UntapStep:
						m_manaSystem.untapStep(activePlayer);
						break;

					case CurrentStep::UpkeepStep:
						removeSummonSickness(activePlayer);
						break;

					case CurrentStep::DrawStep:
						drawCard(activePlayer);
						break;

					case CurrentStep::MainPhaseStep1:
						break;

					case CurrentStep::DeclaringAttackPhaseStep:
						break;

					case CurrentStep::DeclaringAttackersStep:
						break;

					case CurrentStep::DeclaringDefendersStep:
						break;

					case CurrentStep::CombatDamagesStep:
						m_attackSystem.applyDamages();
						m_actionSystem.checkStates(); //Normally, checked each time a player get priority
						player1Lost = std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(m_player1, "PlayerLost"))->data();
						player2Lost = std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(m_player2, "PlayerLost"))->data();
						break;

					case CurrentStep::EndCombatStep:
						break;

					case CurrentStep::MainPhaseStep2:
						break;

					case CurrentStep::EndStep:
						break;

					case CurrentStep::CleanupStep:
						//Discard
						resetDamageTaken();
						m_attackSystem.clearSystem();
						break;

				}

				m_manaSystem.endOfStep();

			}

			else { haveToQuit = true; }
		}
	}

	if(player1Lost) { std::cout << "Player 1 lost !" << std::endl; }
	if(player2Lost) { std::cout << "Player 2 lost !" << std::endl; }
}

void GameSystem::drawXCards(const unsigned int player, const unsigned int x) {

	for(unsigned int i{0}; i < x; i++) { drawCard(player); }
}

void GameSystem::drawCard(const unsigned int player) {

	Entity activePlayer;
	if (player == 0) { activePlayer = m_player1; }
	else { activePlayer = m_player2;  }

	if (std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(activePlayer, "PlayerLooseNextDraw"))->data() == true) {

		std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(activePlayer, "PlayerLost"))->data() = true;
	}

	else {

		std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_library[player].front(), "Area"))->data() = Area::Hand;
		m_library[player].pop_front();
	}
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

		std::shared_ptr<Component> cardStrength{newComponent(ComponentType::Integer)};
		std::static_pointer_cast<IntegerComponent>(cardStrength)->data() = 2;
		m_keeper.addComponent(newCard, "Strength", cardStrength);

		std::shared_ptr<Component> cardToughness{newComponent(ComponentType::Integer)};
		std::static_pointer_cast<IntegerComponent>(cardToughness)->data() = 2;
		m_keeper.addComponent(newCard, "Toughness", cardToughness);

		std::shared_ptr<Component> cardDamageTaken{newComponent(ComponentType::Integer)};
		std::static_pointer_cast<IntegerComponent>(cardDamageTaken)->data() = 0;
		m_keeper.addComponent(newCard, "DamageTaken", cardDamageTaken);
	}

	//Add lands

	for(unsigned int i{0}; i < 48; i++) {

		newCard = m_creator.newEntity();
		m_keeper.addEntity(newCard);

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

	//Add and life points

	std::shared_ptr<Component> lifePoint1{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(lifePoint1)->data() = 20;
	m_keeper.addComponent(m_player1, "LifePoint", lifePoint1);

	std::shared_ptr<Component> lifePoint2{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(lifePoint2)->data() = 20;
	m_keeper.addComponent(m_player2, "LifePoint", lifePoint2);


	//Sort library

	m_library[0] = m_playerCards[0];
	m_library[1] = m_playerCards[1];

	std::random_shuffle(m_library[0].begin(), m_library[0].end());
	std::random_shuffle(m_library[1].begin(), m_library[1].end());
}

void GameSystem::iaAttackAll(const unsigned int activePlayer) {

	Entity entityNotActivePlayer;
	if(activePlayer == 0) {  entityNotActivePlayer = m_player2;}
	else { entityNotActivePlayer = m_player1; }

	std::vector<std::pair<Entity, Entity>> attackingCreatures;

	for(Entity currentEntity: m_playerCards[activePlayer]) {

		if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(currentEntity, "Type"))->data() == CardType::Creature &&
		   std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(currentEntity, "Area"))->data() == Area::Battlefield &&
		   std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(currentEntity, "IsTapped"))->data() == false &&
		   std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(currentEntity, "SummonSickness"))->data() == false) {

			attackingCreatures.emplace_back(currentEntity, entityNotActivePlayer);
		}
	}

	m_attackSystem.declareAttackingCreatures(attackingCreatures);
}

void GameSystem::iaPlayAll(const unsigned int activePlayer) {

	//Play a montain if have 1

	for(unsigned int i{0}; i < m_playerCards[activePlayer].size(); i++) {

		if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "Area"))->data() == Area::Hand && 
		   std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "Type"))->data() == CardType::Land) {

			m_manaSystem.playCard(activePlayer, m_playerCards[activePlayer][i]);
			//Land is put on board if return true, else just return false
		}
	}

	//Tap all lands for mana

	for(unsigned int i{0}; i < m_playerCards[activePlayer].size(); i++) {

		if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "Area"))->data() == Area::Battlefield && 
		  	  std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "Type"))->data() == CardType::Land &&
		  	  std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "IsTapped"))->data() == false) {

			m_manaSystem.tapLandForMana(activePlayer, m_playerCards[activePlayer][i]);
		}
	}

	std::vector<int> manaPool = m_manaSystem.listAllPossibleMana(activePlayer);

	//Play the most possible number of brandon

	for(unsigned int i{0}; i < m_playerCards[activePlayer].size(); i++) {

		if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "Area"))->data() == Area::Hand &&
		   std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "Type"))->data() == CardType::Creature) {

			if(m_manaSystem.playCard(activePlayer, m_playerCards[activePlayer][i])) {

				m_stackSystem.wantToPlay(m_playerCards[activePlayer][i]);
				m_stackSystem.resolveAllSpell();
			}
		}
	}
}

void GameSystem::drawBoard(const unsigned int activePlayer) {

	std::cout << "Player " << activePlayer << " has on hand: " << std::endl;

	for(unsigned int i{0}; i < m_playerCards[activePlayer].size(); i++) {

		if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "Area"))->data() == Area::Hand) {

			std::cout << "  " << std::static_pointer_cast<WordComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "Name"))->data()
			<< " ID: " << m_playerCards[activePlayer][i] << std::endl;
		}
	}

	std::cout << "Player " << activePlayer << " has on board: " << std::endl;

	for(unsigned int i{0}; i < m_playerCards[activePlayer].size(); i++) {

		if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "Area"))->data() == Area::Battlefield) {

			std::cout << "  " << std::static_pointer_cast<WordComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "Name"))->data()
			<< " ID: " << m_playerCards[activePlayer][i] << std::endl;
		}
	}

	Entity entityActivePlayer;
	if(activePlayer == 0) { entityActivePlayer = m_player1; }
	else { entityActivePlayer = m_player2; }

	std::cout << std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(entityActivePlayer, "LifePoint"))->data() << " life points. " << std::endl;
	std::cout << "---------------------------------------------------------------" << std::endl;
}

void GameSystem::resetDamageTaken() {

	for(unsigned int i{0}; i< m_playerCards.size(); i++) {

		for(Entity currentEntity: m_playerCards[i]) {

			if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(currentEntity, "Type"))->data() == CardType::Creature) {

				std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(currentEntity, "DamageTaken"))->data() = 0;
			}
		}
	}
}

void GameSystem::removeSummonSickness(const unsigned int activePlayer) {

	for(unsigned int i{0}; i < m_playerCards[activePlayer].size(); i++) {

		if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "Type"))->data() == CardType::Creature) {

			std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(m_playerCards[activePlayer][i], "SummonSickness"))->data() = false;
		}
	}
}
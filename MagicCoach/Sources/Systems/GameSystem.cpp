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
	m_transmitter{transmitter},
	m_cardLoader{keeper, creator} {

	std::shared_ptr<Component> lifePoint1{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(lifePoint1)->data() = 20;
	m_keeper.addComponent(m_player1, "LifePoint", lifePoint1);

	std::shared_ptr<Component> lifePoint2{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(lifePoint2)->data() = 20;
	m_keeper.addComponent(m_player2, "LifePoint", lifePoint2);
}

void GameSystem::playGame() {

	std::cout << "Emulator: ready !" << std::endl;

	bool player1Lost{false}, player2Lost{false}, haveToQuit{false};
	int activePlayer{0};

	while(!haveToQuit) {

		activePlayer = std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_gameEntity, "ActivePlayer"))->data();

		//TODO: une fonction qui resynchronise vraiment selon les données reçues

		if(m_transmitter.waitingInfoForCoach()) {

			Information lastInfo{m_transmitter.getCoachInfo()};
			lastInfo.player -= 1; //Convert from 1-2 to 0-1

			switch(lastInfo.type) {

				case InformationType::CurrentStep:
					receiveStepInfo(lastInfo);
					break;

				case InformationType::PlayCard:
					std::cout << "Emulator: player want to play " << lastInfo.values[0];
					if(canBePlayed(lastInfo.values[0])) { 
						playCard(lastInfo.values[0], lastInfo.player);
						std::cout << ", can be played." << std::endl;
					}
					else { 
						addToWaitingID(lastInfo.values[0], lastInfo.player); 
						std::cout << ", waiting MTGA ID of this card." << std::endl;
					}
					break;

				case InformationType::SynchroID:
					std::cout << "Emulator: Synchro: " << lastInfo.values[0] << " corresponding to mtgaID: " << lastInfo.values[1] << std::endl;
					addMtgaID(lastInfo.values[0], lastInfo.values[1]);
					checkCardsToPlay();
					break;

				case InformationType::GameOver:
				case InformationType::StopListen:
				case InformationType::MatchOver:
					haveToQuit = true;
					break;


				case InformationType::DeclaringAttackers:
					std::cout << "Emulator: Attaking with " << lastInfo.values.size() << " creatures." << std::endl;
					for (int id : lastInfo.values) {

						if (canBePlayed(id)) {
							Entity attacking = m_cardLoader.getCard(id, lastInfo.player, lastInfo.player);
							m_keeper.drawEntity(attacking);
						}
					}
					break;

				case InformationType::DeclaringBlockers:
					std::cout << "Emulator: Blocking with " << lastInfo.values.size() << " creatures." << std::endl;
					for (int id : lastInfo.values) {

						if (canBePlayed(id)) {
							Entity attacking = m_cardLoader.getCard(id, lastInfo.player, lastInfo.player);
							m_keeper.drawEntity(attacking);
						}
					}
					break;

				case InformationType::Draw:
					break;
			}
		}
	}

	std::cout << "Emulator: end of the game !" << std::endl;

	if(player1Lost) { std::cout << "Player 1 lost !" << std::endl; }
	if(player2Lost) { std::cout << "Player 2 lost !" << std::endl; }
}

void GameSystem::receiveStepInfo(const Information lastInfo) {

	int currentStep = m_stepSystem.getCurrentStep();

	if(lastInfo.values[0] != -1) {

		if(lastInfo.values[0] != currentStep + 1 // Not next step
		&& !(currentStep == CurrentStep::CleanupStep && lastInfo.values[0] == 0) // Not next step
		&& lastInfo.values[0] != currentStep) { //Not current step

			std::cout  << "Emulator: Desynchronized (step " << currentStep << " instead of step " << lastInfo.values[0] << ")." << std::endl;
			std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_gameEntity, "CurrentStep"))->data() = lastInfo.values[0];
			//TODO: resynchoniser également les phases (pas important)
		}

		if (lastInfo.values[0] == currentStep + 1) { //New step

			m_stepSystem.nextStep();
			currentStep = m_stepSystem.getCurrentStep();

			std::cout << "Emulator: Begin of step " << currentStep << std::endl;

			switch(currentStep) {

				case CurrentStep::UntapStep:
					m_manaSystem.untapStep(lastInfo.player);
					break;

				case CurrentStep::UpkeepStep:
					removeSummonSickness(lastInfo.player);
					break;

				case CurrentStep::DrawStep:
					break;

				case CurrentStep::MainPhaseStep1:
					break;

				case CurrentStep::BeginCombat:
					break;

				case CurrentStep::DeclaringAttackersStep:
					break;

				case CurrentStep::DeclaringDefendersStep:
					break;

				case CurrentStep::CombatDamagesStep:
					m_attackSystem.applyDamages();
					m_actionSystem.checkStates(); //Normally, checked each time a player get priority
					break;

				case CurrentStep::EndCombatStep:
					break;

				case CurrentStep::MainPhaseStep2:
					break;

				case CurrentStep::EndStep:
					break;

				case CurrentStep::CleanupStep:
					//Discard
					drawBoard(lastInfo.player);
					resetDamageTaken();
					m_attackSystem.clearSystem();
					break;
			}

			m_manaSystem.endOfStep();
		}
	}
}


void GameSystem::drawBoard(const unsigned int activePlayer) {

	std::cout << "Player " << activePlayer << " has on board: " << std::endl;

	for(unsigned int i{0}; i < m_playerCards[activePlayer].size(); i++) { m_keeper.drawEntity(m_playerCards[activePlayer][i]); }
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

bool GameSystem::canBePlayed(const int instanceID) {

	if(m_correspondenceInstanceMtga.find(instanceID) != m_correspondenceInstanceMtga.end()) {

		if(m_correspondenceInstanceMtga[instanceID] != -1) { return true; }
	}

	return false;
}

void GameSystem::playCard(const int instanceID, const int player) {

	if(canBePlayed(instanceID)) {

		Entity newCard{m_cardLoader.getCard(m_correspondenceInstanceMtga[instanceID], player, player)};
		m_playerCards[player].emplace_back(newCard);
		std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(newCard, "Area"))->data() = Area::Battlefield;
		//drawBoard(player);
	}
}

void GameSystem::addToWaitingID(const int instanceID, const int player) { 
	
	m_idWaitingMtga.emplace(instanceID, player);
	m_correspondenceInstanceMtga.insert(std::make_pair(instanceID, -1));
}

void GameSystem::addMtgaID(const int instanceID, const int mtgaID) {

	if(m_correspondenceInstanceMtga.find(instanceID) == m_correspondenceInstanceMtga.end()) {

		m_correspondenceInstanceMtga.insert(std::make_pair(instanceID, mtgaID));
	}

	else { m_correspondenceInstanceMtga[instanceID] = mtgaID; }
}

void GameSystem::checkCardsToPlay() {

	while(!m_idWaitingMtga.empty() && canBePlayed(m_idWaitingMtga.front().instanceID)) {

		playCard(m_idWaitingMtga.front().instanceID, m_idWaitingMtga.front().player);
		m_idWaitingMtga.pop();
	}
}
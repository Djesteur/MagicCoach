#include "Systems/StateBasedActionSystem.hpp"

StateBasedActionSystem::StateBasedActionSystem(ComponentKeeper &keeper, const Entity currentEntity, const Entity player1, const Entity player2): 
	System{keeper},
	m_gameEntity{currentEntity},
	m_player1{player1},
	m_player2{player2} {

	m_keeper.addEntity(m_gameEntity);
	m_keeper.addEntity(m_player1);
	m_keeper.addEntity(m_player2);

	std::shared_ptr<Component> player1Lost{newComponent(ComponentType::Boolean)};
	std::static_pointer_cast<BooleanComponent>(player1Lost)->data() = false;
	m_keeper.addComponent(m_player1, "PlayerLost", player1Lost);

	std::shared_ptr<Component> player2Lost{newComponent(ComponentType::Boolean)};
	std::static_pointer_cast<BooleanComponent>(player2Lost)->data() = false;
	m_keeper.addComponent(m_player2, "PlayerLost", player2Lost);

	std::shared_ptr<Component> player1LooseNextDraw{newComponent(ComponentType::Boolean)};
	std::static_pointer_cast<BooleanComponent>(player1LooseNextDraw)->data() = false;
	m_keeper.addComponent(m_player1, "PlayerLooseNextDraw", player1LooseNextDraw);

	std::shared_ptr<Component> player2LooseNextDraw{newComponent(ComponentType::Boolean)};
	std::static_pointer_cast<BooleanComponent>(player2LooseNextDraw)->data() = false;
	m_keeper.addComponent(m_player2, "PlayerLooseNextDraw", player2LooseNextDraw);
}

void StateBasedActionSystem::addGameEntity(const Entity entity) { m_gameEntities.emplace_back(entity); }

void StateBasedActionSystem::checkStates() {

	checkLifePoint();
	checkDraw();
	checkDamages();
}

void StateBasedActionSystem::checkLifePoint() {

	if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_player1, "LifePoint"))->data() <= 0) {
		
		std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(m_player1, "PlayerLost"))->data() = true;
	}

	if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_player2, "LifePoint"))->data() <= 0) {

		std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(m_player2, "PlayerLost"))->data() = true;
	}
}

void StateBasedActionSystem::checkDraw() {

	bool player1CanDraw{false}, player2CanDraw{false};

	for(unsigned int i{0}; i < m_gameEntities.size(); i++) {

		if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_gameEntities[i], "Area"))->data() == Area::Library) {

			if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_gameEntities[i], "Controller"))->data() == 0) { player1CanDraw = true; }
			if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_gameEntities[i], "Controller"))->data() == 1) { player2CanDraw = true; }
		}

		if(player1CanDraw && player2CanDraw) { i = m_gameEntities.size(); }
	}

	if(player1CanDraw) { std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(m_player1, "PlayerLooseNextDraw"))->data() = false; }
	else { std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(m_player1, "PlayerLooseNextDraw"))->data() = true; }

	if(player2CanDraw) { std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(m_player2, "PlayerLooseNextDraw"))->data() = false; }
	else { std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(m_player2, "PlayerLooseNextDraw"))->data() = true; }
}

void StateBasedActionSystem::checkDamages() {

	for(Entity currentEntity: m_gameEntities) {

		if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(currentEntity, "Type"))->data() == CardType::Creature) {

			if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(currentEntity, "DamageTaken"))->data() >=
			   std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(currentEntity, "Toughness"))->data()) {

				//Creature die, put it in graveyard
				std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(currentEntity, "Area"))->data() = Area::Graveyard;
			}
		}
	}
}

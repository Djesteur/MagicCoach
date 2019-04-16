#include "Systems/ManaSystem.hpp"

ManaSystem::ManaSystem(ComponentKeeper &keeper, const Entity currentEntity, EntityCreator &creator):
	System{keeper},
	m_gameEntity{currentEntity},
	m_entityCreator{creator},
	m_landPlayedThisTurn{false} {

	m_keeper.addEntity(m_gameEntity);

	m_manaPool[0] = m_entityCreator.newEntity();
	m_manaPool[1] = m_entityCreator.newEntity();
	m_keeper.addEntity(m_manaPool[0]);
	m_keeper.addEntity(m_manaPool[1]);

	std::shared_ptr<Component> poolOwner1{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(poolOwner1)->data() = 0;
	m_keeper.addComponent(m_manaPool[0], "Owner", poolOwner1);
	m_keeper.addComponent(m_manaPool[0], "ManaPool", newComponent(ComponentType::IntegerArray));

	std::shared_ptr<Component> poolOwner2{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(poolOwner2)->data() = 1;
	m_keeper.addComponent(m_manaPool[1], "Owner", poolOwner2);
	m_keeper.addComponent(m_manaPool[1], "ManaPool", newComponent(ComponentType::IntegerArray));
}

bool ManaSystem::playCard(const unsigned int player, const Entity &cardToPlay) {

	//If land

	if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(cardToPlay, "Type"))->data() == CardType::Land) {

		if(!m_landPlayedThisTurn) { 

			std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(cardToPlay, "Area"))->data() = Area::Battlefield;
			m_playerLand[player].emplace_back(cardToPlay);
			m_landPlayedThisTurn = true;
			return true; 
		}

		else { return false; }
	}

	//Other cards type

	std::vector<int> manaCost{std::static_pointer_cast<IntegerArrayComponent>(m_keeper.getComponent(cardToPlay, "ManaCost"))->data()},
					 needToPay{manaCost},
					 &manaPool{std::static_pointer_cast<IntegerArrayComponent>(m_keeper.getComponent(m_manaPool[player], "ManaPool"))->data()};

	//Take mana from mana pool.

	bool foundMana{false};

	unsigned int anyColorToPay{0};

	while(!needToPay.empty()) {

		foundMana = false;

		if(needToPay[0] != Mana::AnyColor) { //Will be pay in a 2nd time

			for(unsigned int j{0}; j < manaPool.size() && !foundMana; j++) {

				if(needToPay[0] == manaPool[j]) {

					foundMana = true;
					needToPay.erase(needToPay.begin());
					manaPool.erase(manaPool.begin() + j);
				}
			}
		}

		else { 

			foundMana = true; 
			anyColorToPay++;
			needToPay.erase(needToPay.begin());
		}

		if(!foundMana) { return false; }
	}

	if(anyColorToPay > manaPool.size()) { return false; }
	else {

		while(anyColorToPay > 0) {

			anyColorToPay--;
			manaPool.erase(manaPool.begin());
		}

	}

	return true;
}



void ManaSystem::untapStep(const unsigned int player) {

	m_landPlayedThisTurn = false;

	for(unsigned int i{0}; i < m_playerLand[player].size(); i++) {

		std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(m_playerLand[player][i], "IsTapped"))->data() = false;
	}
}

void ManaSystem::endOfStep() {

	std::static_pointer_cast<IntegerArrayComponent>(m_keeper.getComponent(m_manaPool[0], "ManaPool"))->data().clear();
	std::static_pointer_cast<IntegerArrayComponent>(m_keeper.getComponent(m_manaPool[1], "ManaPool"))->data().clear();
}

std::vector<int> ManaSystem::listAllPossibleMana(const unsigned int player) const {

	std::vector<int> manaPool{std::static_pointer_cast<IntegerArrayComponent>(m_keeper.getComponent(m_manaPool[player], "ManaPool"))->data()},
					  result{manaPool};

	for(unsigned int i{0}; i < m_playerLand[player].size(); i++) {

		if(std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(m_playerLand[player][i], "IsTapped"))->data() == false) {

			result.emplace_back(getManaFromName(std::static_pointer_cast<WordComponent>(m_keeper.getComponent(m_playerLand[player][i], "Name"))->data()));
		}
	}

	return result;
}

int ManaSystem::getManaFromName(const std::string name) const {

	if(name == "Plain") { return Mana::White; }
	if(name == "Island") { return Mana::Blue; }
	if(name == "Swamp") { return Mana::Black; }
	if(name == "Mountain") { return Mana::Red; }
	if(name == "Forest") { return Mana::Green; }

	return Mana::Colorless;
}

void ManaSystem::tapLandForMana(const unsigned int player, const Entity &land) {

	if(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(land, "Type"))->data() == CardType::Land
	&& std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(land, "IsTapped"))->data() == false) {

		std::static_pointer_cast<IntegerArrayComponent>(m_keeper.getComponent(m_manaPool[player], "ManaPool"))->data().emplace_back(
		getManaFromName(std::static_pointer_cast<WordComponent>(m_keeper.getComponent(land, "Name"))->data()));

		std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(land, "IsTapped"))->data() = true;
	}
}
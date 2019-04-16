#include "Systems/AttackSystem.hpp"

AttackSystem::AttackSystem(ComponentKeeper &keeper, const Entity player1, const Entity player2):
	System{keeper},
	m_player1{player1},
	m_player2{player2} {}

void AttackSystem::declareAttackingCreatures(const std::vector<std::pair<Entity, Entity>> attackingCreatures) {

	m_attackingCreatures.insert(m_attackingCreatures.end(), attackingCreatures.begin(), attackingCreatures.end());
}

void AttackSystem::declareBlockingCreatures(const std::vector<std::pair<Entity, Entity>> blockingCreatures) {

	m_blockingCreatures.insert(m_blockingCreatures.end(), blockingCreatures.begin(), blockingCreatures.end());	
}

std::vector<std::pair<Entity, Entity>> AttackSystem::getAttackingCreatures() const { return m_attackingCreatures; }
std::vector<std::pair<Entity, Entity>> AttackSystem::getBlockingCreatures() const { return m_blockingCreatures; }

void AttackSystem::applyDamages() {

	std::vector<bool> isBlocked;
	isBlocked.resize(m_attackingCreatures.size());
		//std::cout << "ATTACK " << m_attackingCreatures.size() << " " << isBlocked.size() << std::endl;

	//Define which creature is blocked

	for(std::pair<Entity, Entity> blocking: m_blockingCreatures) {

		for(unsigned int i{0}; i < isBlocked.size(); i++) {

			if(blocking.second == m_attackingCreatures[i].first) { isBlocked[i] = true; }
		}
	}

	//Apply attacking creature damages

	for(unsigned int i{0}; i < m_attackingCreatures.size(); i++) {

		if(isBlocked[i]) {

			//TODO: with declaration of splited damages
		}

		else {

			if(m_attackingCreatures[i].second == m_player1) {

				std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_player1, "LifePoint"))->data() -= 
				std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_attackingCreatures[i].first, "Strength"))->data();
			}

			else if(m_attackingCreatures[i].second == m_player2) {

				std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_player2, "LifePoint"))->data() -= 
				std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_attackingCreatures[i].first, "Strength"))->data();
			}

			//If isn't a player, the last target can only be a planeswalker
			else {}
		}
	}

	//Apply blocking creature damages

	for(std::pair<Entity, Entity> blocking: m_blockingCreatures) {

		std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(blocking.second, "Damages"))->data() += 
		std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(blocking.first, "Strength"))->data();
	}
}

void AttackSystem::clearSystem() {

	m_attackingCreatures.clear();
	m_blockingCreatures.clear();
}
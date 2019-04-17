#ifndef ATTACKSYSTEM_HPP
#define ATTACKSYSTEM_HPP

#include <deque>
#include <array>
#include <string>

#include "Systems/System.hpp"
#include "GameTypes.hpp"

class AttackSystem : public System {

	public:

		AttackSystem(ComponentKeeper &keeper, const Entity player1, const Entity player2);

		void declareAttackingCreatures(const std::vector<std::pair<Entity, Entity>> attackingCreatures);
		void declareBlockingCreatures(const std::vector<std::pair<Entity, Entity>> blockingCreatures);

		std::vector<std::pair<Entity, Entity>> getAttackingCreatures() const;
		std::vector<std::pair<Entity, Entity>> getBlockingCreatures() const;

		void applyDamages();
		void clearSystem();

		//TODO: select damages to do between attacking/blocking creatures
		//TODO: take in consideration various combat effect: reach, trample, first strike, double strike, ...

	private:

		Entity m_player1, m_player2;

		std::vector<std::pair<Entity, Entity>> m_attackingCreatures; //First: attacking creature, second: target
		std::vector<std::pair<Entity, Entity>> m_blockingCreatures; //First: blocking creature, second: attacking creature
};

#endif
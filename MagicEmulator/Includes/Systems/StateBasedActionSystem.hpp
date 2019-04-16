#ifndef STATEBASEDACTIONSYSTEM_HPP
#define STATEBASEDACTIONSYSTEM_HPP

#include <deque>
#include <array>
#include <string>

#include "Systems/System.hpp"
#include "GameTypes.hpp"

class StateBasedActionSystem : public System {

	public:

		StateBasedActionSystem(ComponentKeeper &keeper, const Entity currentEntity, const Entity player1, const Entity player2);

		void addGameEntity(const Entity entity);
		void checkStates();

	private:

		void checkLifePoint();
		void checkDraw();
		void checkDamages();

		Entity m_gameEntity, m_player1, m_player2;
		std::vector<Entity> m_gameEntities;
};

/*Actions to do:

	- Player's life points 					- OK
	- Draw with empty library 				- OK
	- Number of poison marker				- Todo
	- Token area							- Todo
	- Cast spell area						- Todo
	- Creatures toughtness					- Todo
	- Creatures damages 					- OK
	- Cratures take deathtouch damages 		- Todo
	- Planeswalker loyalty					- Todo
	- Legendary rule						- Todo
	- World rule							- Todo
	- Aura attach							- Todo
	- Equipment attach						- Todo
	- Creature attach						- Todo
	- +1/+1 and -1/-1 counter 				- Todo
	- Counters in excess + lore counter 	- Todo
*/
#endif
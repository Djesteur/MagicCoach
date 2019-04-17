#ifndef STACKSYSTEM_HPP
#define STACKSYSTEM_HPP

#include <stack>

#include "Systems/System.hpp"
#include "GameTypes.hpp"

class StackSystem : public System {

	public:

		StackSystem(ComponentKeeper &keeper, const Entity currentEntity);

		bool wantToPlay(const Entity entityToPlay);

		void resolveNextSpell();
		void resolveAllSpell(); 

		unsigned int nbEffectOnStack() const;

	private:

		Entity m_gameEntity;

		std::stack<Entity> m_stack;
};

#endif
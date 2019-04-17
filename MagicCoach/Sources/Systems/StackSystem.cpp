#include "Systems/StackSystem.hpp"

StackSystem::StackSystem(ComponentKeeper &keeper, const Entity currentEntity): 
	System{keeper},
	m_gameEntity{currentEntity} {

	m_keeper.addEntity(m_gameEntity);
}

bool StackSystem::wantToPlay(const Entity entityToPlay) { 

	std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(entityToPlay, "Area"))->data() = Area::Stack;
	m_stack.push(entityToPlay);
	return true; 
}

void StackSystem::resolveNextSpell() {

	switch(std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_stack.top(), "Type"))->data()) {

		case CardType::Creature:
			std::static_pointer_cast<IntegerComponent>(m_keeper.getComponent(m_stack.top(), "Area"))->data() = Area::Battlefield;
			std::static_pointer_cast<BooleanComponent>(m_keeper.getComponent(m_stack.top(), "SummonSickness"))->data() = true;
			break;

		case CardType::Artifact:
			break;

		case CardType::Enchantment:
			break;

		case CardType::Planeswalker:
			break;

		case CardType::Sorcery:
			break;

		case CardType::Instant:
			break;

		default:
			break;
	}

	m_stack.pop();
}

void StackSystem::resolveAllSpell() {

	while(!m_stack.empty()) { resolveNextSpell(); }
}

unsigned int StackSystem::nbEffectOnStack() const { return m_stack.size(); }
#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <vector>
#include <algorithm>

#include "EntityCreator.hpp"
#include "ComponentKeeper.hpp"
#include "Components/ComponentUtils.hpp"

class System {

	public:

		System(ComponentKeeper &keeper);

		virtual ~System() = 0;

		virtual void addEntity(const Entity &currentEntity);
		virtual void deleteEntity(const Entity &currentEntity);

		bool isInSystem(const Entity &currentEntity) const;

	protected:

		ComponentKeeper &m_keeper;
		std::vector<Entity> m_entities;
};

#endif
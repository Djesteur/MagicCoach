#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <vector>
#include <algorithm>

#include "EntityCreator.hpp"

class System {

	public:

		System();

		virtual ~System() = 0;

		virtual void addEntity(const Entity &currentEntity) = 0;
		virtual void deleteEntity(const Entity &currentEntity);

		bool isInSystem(const Entity &currentEntity) const;

	protected:

		std::vector<Entity> m_entities;
};

#endif
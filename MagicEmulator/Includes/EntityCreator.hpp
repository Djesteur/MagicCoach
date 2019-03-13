#ifndef ENTITYCREATOR_HPP
#define ENTITYCREATOR_HPP

#include <cstdint>

using Entity = uint32_t;

class EntityCreator {

	public:

		EntityCreator();
		EntityCreator(const EntityCreator&) = delete;
		EntityCreator &operator=(const EntityCreator&) = delete;

		Entity newEntity();

	private:

		Entity m_lastEntity;
};

#endif
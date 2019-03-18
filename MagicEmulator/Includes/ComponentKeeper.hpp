#ifndef COMPONENTKEEPER_HPP
#define COMPONENTKEEPER_HPP

#include <memory>
#include <map>
#include <vector>

#include "Components/Component.hpp"

using Entity = uint32_t;

struct ComponentHolder {

	std::string name;
	std::shared_ptr<Component> component;
};

class ComponentKeeper {

	public:

		ComponentKeeper();
		ComponentKeeper(const ComponentKeeper&) = delete;
		ComponentKeeper &operator=(const ComponentKeeper&) = delete;

		void addEntity(const Entity newEntity);
		void deleteEntity(const Entity currentEntity);
		bool existingEntity(const Entity currentEntity) const;

		void addComponentToEntity(const Entity currentEntity, const std::string componentName, const Component &newComponent);
		void deleteComponentToEntity(const Entity currentEntity, const std::string componentName);

		void changeComponentOfEntity(const Entity currentEntity, const std::string componentName, std::shared_ptr<Component> newComponent);
		std::shared_ptr<Component> getComponentOfEntity(const Entity currentEntity, const std::string componentName);

	private:

		std::map<Entity, std::vector<ComponentHolder>> m_components;
};

#endif
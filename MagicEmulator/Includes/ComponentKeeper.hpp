#ifndef COMPONENTKEEPER_HPP
#define COMPONENTKEEPER_HPP

#include <memory>
#include <map>
#include <vector>
#include <iostream>
#include <exception>

#include "Components/Component.hpp"
#include "Components/ComponentUtils.hpp"

#include "EntityCreator.hpp"

class ComponentKeeper {

	public:

		ComponentKeeper();
		ComponentKeeper(const ComponentKeeper&) = delete;
		ComponentKeeper &operator=(const ComponentKeeper&) = delete;

		void addEntity(const Entity newEntity);
		void deleteEntity(const Entity currentEntity);
		bool existingEntity(const Entity currentEntity) const;

		void addComponent(const Entity currentEntity, const std::string componentName, const std::shared_ptr<Component> &newComponent);
		void deleteComponent(const Entity currentEntity, const std::string componentName);

		void changeComponent(const Entity currentEntity, const std::string componentName, std::shared_ptr<Component> newComponent);
		std::shared_ptr<Component> getComponent(const Entity currentEntity, const std::string componentName);

		bool entityHasComponent(const Entity currentEntity, const std::string componentName);

		void drawEntity(const Entity &entity);
		void drawComponents();

	private:

		std::map<Entity, std::map<std::string, std::shared_ptr<Component>>> m_components;
};

#endif
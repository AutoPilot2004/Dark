#pragma once
#include <entt/entt.hpp>
#include "Scene/Scene.h"
#include "Components.h"

#include <iostream>

namespace Dark
{
	class Entity
	{
	public:
		Entity() : m_entityID((entt::entity)-1), m_scene(nullptr) {}
		Entity(Scene* scene);
		Entity(Scene* scene, std::uint32_t entityID);
		~Entity();

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			if (hasComponent<T>()) {
				std::cout << "Already has Component!\n";
				return getComponent<T>();
			}

			return m_scene->m_registry.emplace<T>(m_entityID, std::forward<Args>(args)...);
		}

		template<typename T>
		void removeComponent()
		{
			m_scene->m_registry.remove<T>(m_entityID);
		}

		template<typename T>
		bool hasComponent()
		{
			return m_scene->m_registry.all_of<T>(m_entityID);
		}
		template<typename T, typename... Args>
		bool hasComponents()
		{
			return m_scene->m_registry.all_of<T, Args...>(m_entityID);
		}

		template<typename T, typename... Args>
		bool hasAnyOfComponents()
		{
			return m_scene->m_registry.any_of<T, Args...>(m_entityID);
		}

		template<typename T>
		T& getComponent()
		{
			if (!hasComponent<T>()) {
				std::cout << "Doesn't have Component!\n";
			}

			return *m_scene->m_registry.try_get<T>(m_entityID);
		}

		//Getters
		inline int getID()
		{
			if (m_scene)
				return getComponent<IdentifierComponent>().id;
			else
				return -1;
		}

		operator bool() const {
			return ((m_entityID != (entt::entity)-1)&&(m_scene));
		}

		operator uint64_t() const {
			return (uint64_t)m_entityID;
		}

	private:
		entt::entity m_entityID;

		Scene* m_scene;
	};
}
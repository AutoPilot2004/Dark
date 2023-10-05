#include "Entity.h"

namespace Dark
{
	Entity::Entity(Scene* scene)
		: m_scene(scene)
	{
		m_entityID = m_scene->m_registry.create();
	}

	Entity::Entity(Scene* scene, std::uint32_t entityID)
		: m_entityID((entt::entity)entityID), m_scene(scene)
	{
		//empty
	}

	Entity::~Entity()
	{
		//empty
	}
}
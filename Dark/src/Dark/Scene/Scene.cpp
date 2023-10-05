#include "Scene.h"

#include "SceneManager.h"

#include "Entity.h"
#include "Components.h"

#include "Renderer/Renderer2D.h"
#include "Graphics/TextureAtlas.h"
#include "Utils/ResourceManager.h"

#include <string>

namespace Dark
{
	Entity Scene::createEntity()
	{
		Entity entity = { this };

		entity.addComponent<IdentifierComponent>();
		entity.addComponent<TagComponent>();
		entity.addComponent<RendererComponent>();
		entity.addComponent<TransformComponent>();

		return entity;
	}

	Entity Scene::createEntity(const glm::vec3& position)
	{
		Entity entity = { this };

		entity.addComponent<IdentifierComponent>();
		entity.addComponent<TagComponent>();
		entity.addComponent<RendererComponent>();
		entity.addComponent<TransformComponent>(position);

		return entity;
	}

	Entity Scene::createEntity(const glm::vec3& position, float rotation)
	{
		Entity entity = { this };

		entity.addComponent<IdentifierComponent>();
		entity.addComponent<TagComponent>();
		entity.addComponent<RendererComponent>();
		entity.addComponent<TransformComponent>(position, rotation);

		return entity;
	}

	Entity Scene::createEntity(const glm::vec3& position, float rotation, const glm::vec2& scale)
	{
		Entity entity = { this };

		entity.addComponent<IdentifierComponent>();
		entity.addComponent<TagComponent>();
		entity.addComponent<RendererComponent>();
		entity.addComponent<TransformComponent>(position, rotation, scale);

		return entity;
	}

	Entity Scene::createEntityWithID(int id)
	{
		Entity entity = { this };

		entity.addComponent<IdentifierComponent>(id);
		entity.addComponent<TagComponent>();
		entity.addComponent<RendererComponent>();
		entity.addComponent<TransformComponent>();

		return entity;
	}

	void Scene::deleteEntity(std::uint64_t entityID)
	{
		m_registry.destroy((entt::entity)entityID);
	}

	Entity Scene::createCamera()
	{
		Entity entity = { this };

		static const Texture& cameraTex = ResourceManager::getTexture("resources/textures/camera.png");

		entity.addComponent<IdentifierComponent>();
		entity.addComponent<TagComponent>().tag = "Camera";
		entity.addComponent<RendererComponent>().texture = &cameraTex;
		entity.addComponent<TransformComponent>();
		entity.addComponent<CameraComponent>();

		return entity;
	}

	Entity Scene::createCameraWithID(int id)
	{
		Entity entity = { this };

		static const Texture& cameraTex = ResourceManager::getTexture("resources/textures/camera.png");

		entity.addComponent<IdentifierComponent>(id);
		entity.addComponent<TagComponent>().tag = "Camera";
		entity.addComponent<RendererComponent>().texture = &cameraTex;
		entity.addComponent<TransformComponent>();
		entity.addComponent<CameraComponent>();

		return entity;
	}

	Entity Scene::getEntityWithID(std::uint32_t entityID)
	{
		return { this, entityID };
	}

	bool Scene::camChanged()
	{
		if (m_prevCamera == m_activeCamera)
			return false;

		m_prevCamera = m_activeCamera;

		return true;
	}
}
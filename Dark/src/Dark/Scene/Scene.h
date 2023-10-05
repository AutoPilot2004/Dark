#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "Camera2D.h"
#include "Graphics/FrameBuffer.h"

namespace Dark
{
	class Entity;

	class Scene
	{
	public:
		Scene() = default;
		virtual ~Scene() = default;

		virtual void onInit() = 0;
		virtual void onDestroy() = 0;
		virtual void onEntry() = 0;
		virtual void onExit() = 0;
		virtual void onUpdate(float dT) = 0;
		virtual void onRender() = 0;

		bool camChanged();

		//Getters
		inline Camera2D* getActiveCamera() const { return m_activeCamera; }
		inline FrameBuffer* getFBO() const { return m_fbo.get(); }

	protected:
		Entity createEntity();
		Entity createEntity(const glm::vec3& position);
		Entity createEntity(const glm::vec3& position, float rotation);
		Entity createEntity(const glm::vec3& position, float rotation, const glm::vec2& scale);
		Entity createEntityWithID(int id);

		void deleteEntity(std::uint64_t entityID);

		Entity createCamera();
		Entity createCameraWithID(int id);

		Entity getEntityWithID(std::uint32_t entityID);


	protected:
		entt::registry m_registry;

		Camera2D* m_activeCamera = nullptr;

		Camera2D m_editorCamera;

		std::unique_ptr<FrameBuffer> m_fbo;

	private:
		Camera2D* m_prevCamera = m_activeCamera;

		friend class Entity;
		friend class SceneSerializer;
	};
}
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <box2d/box2d.h>

#include "Graphics/Vertex.h"
#include "Graphics/Texture.h"
#include "Scene/Camera2D.h"

#include <string>

namespace Dark
{
	namespace Components
	{
		extern void resetStaticVars();
	}

	//Environment
	struct Environment
	{
		Environment() = default;
		~Environment() = default;

		static Color backgroundColor;
		static b2Vec2 gravity;
	};

	//Entities
	struct IdentifierComponent
	{
		IdentifierComponent() { id = idCounter++; }
		IdentifierComponent(int id) { this->id = id; }
		~IdentifierComponent() = default;

		int id;

		//DO NOT USE
		static int idCounter;
	};

	struct TagComponent
	{
		TagComponent() = default;
		TagComponent(const std::string& tag)
			: tag(tag) {}
		~TagComponent() = default;

		std::string tag = "Entity";
	};

	struct RendererComponent
	{
		RendererComponent() = default;
		RendererComponent(Texture* texture)
			: texture(texture) {}
		RendererComponent(const Color& color)
			: color(color) {}
		RendererComponent(Texture* texture, const Color& color)
			: texture(texture), color(color) {}
		~RendererComponent() = default;

		const Texture* texture = nullptr;
		Color color = { 1.0f, 1.0f, 1.0f, 1.0f };

		bool isTextureAtlas = false;
		int index = 0;
		int rows = 1;
		int cols = 1;
		float offsetX = 0.0f;
		float offsetY = 0.0f;

		bool isVisible = true;
	};

	struct TransformComponent
	{
		TransformComponent() = default;
		TransformComponent(const glm::vec3& translation)
			: translation(translation) {}
		TransformComponent(const glm::vec3& translation, float rotation)
			: translation(translation), rotation(rotation) {}
		TransformComponent(const glm::vec3& translation, float rotation, const glm::vec2& scale)
			: translation(translation), rotation(rotation), scale(scale) {}
		~TransformComponent() = default;

		void setTranslation(const glm::vec3& translation)
		{
			this->translation = translation;
			updateTranslationMat();
		}

		void setTranslation2f(const glm::vec2& translation)
		{
			this->translation.x = translation.x;
			this->translation.y = translation.y;
			updateTranslationMat();
		}

		void setRotation(float rotationInDegrees)
		{
			this->rotation = rotationInDegrees;
			updateRotationMat();
		}

		void setScale(const glm::vec2& scale)
		{
			this->scale = scale;
			updateScaleMat();
		}

		void updateTranslationMat()
		{
			m_transMat = glm::translate(glm::mat4(1.0f), translation);
			m_isDirty = true;
		}

		void updateRotationMat()
		{
			m_rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
			m_isDirty = true;
		}

		void updateScaleMat()
		{
			m_scaleMat = glm::scale(glm::mat4(1.0f), { scale, 1.0f });
			m_isDirty = true;
		}

		void updateModelMatrix()
		{
			if (m_isDirty) {
				m_modelMat = m_transMat * m_rotMat * m_scaleMat;
				m_isDirty = false;
			}
		}

		inline const glm::mat4& getModelMatrix() const { return m_modelMat; }

		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
		float rotation = 0.0f;
		glm::vec2 scale = glm::vec2(1.0f, 1.0f);

	private:
		glm::mat4 m_modelMat = glm::mat4(1.0f);

		glm::mat4 m_transMat = glm::mat4(1.0f);
		glm::mat4 m_rotMat = glm::mat4(1.0f);
		glm::mat4 m_scaleMat = glm::mat4(1.0f);

		bool m_isDirty = true;
	};

	struct CameraComponent
	{
		CameraComponent() = default;
		~CameraComponent() = default;

		Camera2D camera;

		static int active;
	};

	enum BodyType
	{
		STATIC,
		DYNAMIC,
		KINEMATIC
	};

	struct Rigidbody2DComponent
	{
		Rigidbody2DComponent() = default;
		~Rigidbody2DComponent() = default;

		b2Body* body = nullptr;
		b2BodyDef bodyDef;
		int bodyType = BodyType::STATIC;
		//bool fixedRotation = false;
	};

	struct BoxCollider2DComponent
	{
		BoxCollider2DComponent() = default;
		~BoxCollider2DComponent() = default;

		b2PolygonShape polygonShape;
		b2FixtureDef fixtureDef;
		//b2Fixture* fixture = nullptr;
		//float density = 1.0f;
		//float friction = 0.3f; //< [0..1]
		//float restitution = 0.0f; //< [0..1]
		//float restitutionThreshold = 0.5f;
	};
}
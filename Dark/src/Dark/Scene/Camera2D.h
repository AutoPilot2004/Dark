#pragma once
#include <glm/gtc/matrix_transform.hpp>

namespace Dark
{
	class Camera2D
	{
	public:
		Camera2D();

		void resize(int screenWidth, int screenHeight);

		void setPosition(const glm::vec2& position) { m_position = position; updateCameraMatrix(); }
		void setRotation(float degrees) { m_rotation = degrees; updateCameraMatrix(); }
		void setScale(float scale) { m_scale = scale; updateCameraMatrix(); }

		void offsetPosition(const glm::vec2& position) { m_position += position; updateCameraMatrix(); }
		void offsetRotation(float degrees) { m_rotation += degrees; updateCameraMatrix(); }
		void offsetScale(float scale) { m_scale += scale; updateCameraMatrix(); }

		glm::vec2 screenToWorldCoords(glm::vec2 screenCoords) const;

		//Getters
		inline const glm::mat4& getCameraMatrix() const { return m_cameraMatrix; }

		inline const glm::vec2& getPosition() const { return m_position; }
		inline float getRotation() const { return m_rotation; }
		inline float getScale() const { return m_scale; }

	private:
		void updateCameraMatrix();

	private:
		int m_sWidth, m_sHeight;

		glm::vec2 m_position;
		float m_rotation;
		float m_scale;

		glm::mat4 m_projMatrix;
		glm::mat4 m_cameraMatrix;
	};
}
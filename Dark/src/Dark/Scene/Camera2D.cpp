#include "Camera2D.h"

#include <iostream>

namespace Dark
{
	Camera2D::Camera2D()
		: m_sWidth(1920), m_sHeight(1080), m_position(0.0f), m_rotation(0.0f), m_scale(1.0f), m_projMatrix(1.0f), m_cameraMatrix(1.0f)
	{
		//empty
	}

	void Camera2D::resize(int screenWidth, int screenHeight)
	{
		m_sWidth = screenWidth;
		m_sHeight = screenHeight;

		float aR = (float)m_sWidth / m_sHeight;

		//FIX
		m_projMatrix = glm::ortho(-m_sWidth / 64.0f, m_sWidth / 64.0f, -m_sHeight / 64.0f, m_sHeight / 64.0f, 0.0f, -100.001f);

		updateCameraMatrix();
	}

	void Camera2D::updateCameraMatrix()
	{
		glm::mat4 view(1.0f);
		view = glm::rotate(view, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		view = glm::scale(view, glm::vec3(m_scale, m_scale, 1.0f));
		view = glm::translate(view, glm::vec3(-m_position.x, -m_position.y, 0.0f));

		m_cameraMatrix = m_projMatrix * view;
	}

	glm::vec2 Camera2D::screenToWorldCoords(glm::vec2 screenCoords) const
	{
		screenCoords.y = m_sHeight - screenCoords.y;
		screenCoords -= glm::vec2(m_sWidth / 2.0f, m_sHeight / 2.0f);
		screenCoords /= 32;
		screenCoords /= m_scale;
		screenCoords += m_position;

		return screenCoords;
	}
}
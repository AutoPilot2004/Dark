#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "ECheck/ECheck.h"

namespace Dark
{
	struct Texture
	{
		Texture() = default;
		virtual ~Texture() = default;

		inline const glm::vec4& getUV(int index, int rows, int cols, float offset_x, float offset_y)
		{
			if (m_isDirty) {
				float perRow = (float)m_height / (float)rows;
				float perCol = (float)m_width / (float)cols;

				m_uvCoords = glm::vec4(
					((float)((index % cols) * perCol) / (float)m_width) + offset_x,
					((float)((m_height - ((index / cols) * perRow)) - perRow) / (float)m_height) + offset_y,
					((float)perCol / (float)m_width) - (offset_x * 2),
					((float)perRow / (float)m_height) - (offset_y * 2));

				m_isDirty = false;
			}

			return m_uvCoords;
		}

		void isDirty(bool dirty = true) const
		{
			const_cast<bool&>(m_isDirty) = dirty;
		}

		//Getters
		inline const std::string& getFilePath() const { return m_filePath; }
		inline GLuint getID() const { return m_id; }
		inline int getWidth() const { return m_width; }
		inline int getHeight() const { return m_height; }

		operator GLuint() const
		{
			return m_id;
		}

	protected:
		GLuint m_id;
		std::string m_filePath;
		int m_width, m_height;
		int m_channels;
		glm::vec4 m_uvCoords = glm::vec4(1.0f);

		bool m_isDirty = true;

	private:

		friend class ResourceManager;
	};
}

namespace std
{
	template<>
	struct hash<Dark::Texture>
	{
		size_t operator()(const Dark::Texture& key) const
		{
			return hash<GLuint>()(key);
		}
	};
}
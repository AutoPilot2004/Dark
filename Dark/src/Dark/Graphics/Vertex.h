#pragma once
#include <glm/glm.hpp>

namespace Dark
{
	struct Color
	{
		float r;
		float g;
		float b;
		float a;

		float& operator[](int i) {
			return reinterpret_cast<float*>(this)[i];
		}
	};

	struct Vertex
	{
		glm::vec3 position;

		Color color;

		glm::vec2 uv;

		float textureIndex;

		void setPosition(const glm::vec3& position)
		{
			this->position = position;
		}

		void setColor(const Color& color)
		{
			this->color = color;
		}

		void setUV(const glm::vec2& uv)
		{
			this->uv = uv;
		}

		void setTextureIndex(float index)
		{
			this->textureIndex = index;
		}
	};
}
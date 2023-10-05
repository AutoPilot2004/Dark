#pragma once
#include "Graphics/Graphics.h"
#include "Graphics/ShaderProgram.h"

#include <glm/glm.hpp>
#include <unordered_map>

#include "Scene/Components.h"

namespace Dark
{
	struct Quad;

	class Renderer2D
	{
	public:
		static void init(ShaderProgram& shader);
		static void destroy();

		static void begin();
		static void drawQuad(const Texture& texture, const Color& color, const glm::mat4& transform);
		static void drawQuad(const Texture& texture, const Color& color, const glm::mat4& transform, const glm::vec4& uv);
		static void drawQuad(const Color& color, const glm::mat4& transform);
		static void drawQuad(const Texture& texture, const Color& color, const glm::vec3& position, float rotationInDegrees, const glm::vec2& size, const glm::vec4& uv);
		static void drawQuad(const Texture& texture, const glm::vec3& position, float rotationInDegrees, const glm::vec2& size, const glm::vec4& uv);
		static void drawQuad(const Color& color, const glm::vec3& position, float rotationInDegrees, const glm::vec2& size);
		static void end();
		static void flush();


	private:
		Renderer2D() = delete;

		static bool sortByDepth(Quad a, Quad b);

	private:
		static Quad* m_quads;
		static Quad* m_currentQuad;

		static glm::vec4 m_localCoords[4];

		static VertexBuffer m_vbo;
		static IndexBuffer m_ibo;
		static VertexArray m_vao;

		static float m_textureUnitIndex;
		static std::unordered_map<Texture, float> m_textureUnitMap;

		static int m_vertexCount;
		static int m_indexCount;
	};
}
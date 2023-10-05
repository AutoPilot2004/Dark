#include "Renderer2D.h"

#include "ECheck/ECheck.h"
#include "Utils/ResourceManager.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

static constexpr int MAX_QUAD_COUNT = 20000;
static constexpr int MAX_VERTEX_COUNT = MAX_QUAD_COUNT << 2;
static constexpr int MAX_INDEX_COUNT = MAX_QUAD_COUNT * 6;

static constexpr float MAX_TEXTURE_UNITS = 32.0f;

namespace Dark
{
	struct Quad
	{
		Vertex bottomLeft;
		Vertex bottomRight;
		Vertex topRight;
		Vertex topLeft;
	};

	Quad* Renderer2D::m_quads = nullptr;
	Quad* Renderer2D::m_currentQuad = nullptr;

	glm::vec4 Renderer2D::m_localCoords[4];

	VertexBuffer Renderer2D::m_vbo;
	IndexBuffer Renderer2D::m_ibo;
	VertexArray Renderer2D::m_vao;

	float Renderer2D::m_textureUnitIndex;
	std::unordered_map<Texture, float> Renderer2D::m_textureUnitMap;

	int Renderer2D::m_vertexCount;
	int Renderer2D::m_indexCount;

	void Renderer2D::init(ShaderProgram& shader)
	{
		m_quads = new Quad[MAX_QUAD_COUNT];

		m_localCoords[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		m_localCoords[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		m_localCoords[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		m_localCoords[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		m_vbo.gen();
		m_vbo.bind();
		m_vbo.allocMemory(MAX_VERTEX_COUNT * sizeof(Vertex), GL_DYNAMIC_DRAW);

		m_ibo.gen();
		m_ibo.bind();
		m_ibo.uploadDataCount(MAX_INDEX_COUNT);

		m_vao.gen();
		m_vao.addVertexAttribute<float, false>(3);
		m_vao.addVertexAttribute<float, false>(4);
		m_vao.addVertexAttribute<float, false>(2);
		m_vao.addVertexAttribute<float, false>(1);

		m_vao.build(m_vbo, m_ibo);

		m_textureUnitIndex = 1.0f;
		m_textureUnitMap[ResourceManager::getWhiteTexture()] = 0.0f;

		m_vertexCount = 0;
		m_indexCount = 0;

		int samplers[int(MAX_TEXTURE_UNITS)];

		for (int i = 0; i < MAX_TEXTURE_UNITS; i++) {
			samplers[i] = i;
		}


		shader.bind();
		shader.setUniform1iv("tex", MAX_TEXTURE_UNITS, samplers);
	}

	void Renderer2D::destroy()
	{
		delete[] m_quads;

		m_vbo.del();
		m_ibo.del();
		m_vao.del();
	}

	void Renderer2D::begin()
	{
		m_currentQuad = m_quads;

		m_vertexCount = 0;
		m_indexCount = 0;
	}

	void Renderer2D::drawQuad(const Texture& texture, const Color& color, const glm::mat4& transform)
	{
		if (m_vertexCount >= MAX_VERTEX_COUNT || m_textureUnitIndex >= MAX_TEXTURE_UNITS) {
			end();
			flush();
			begin();
		}

		if (m_textureUnitMap.find(texture) == m_textureUnitMap.end()) {
			m_textureUnitMap[texture] = m_textureUnitIndex++;
		}

		float textureIndex = m_textureUnitMap.at(texture);

		m_currentQuad->bottomLeft.setPosition(transform * m_localCoords[0]);
		m_currentQuad->bottomLeft.setColor(color);
		m_currentQuad->bottomLeft.setUV({ 0.0f, 0.0f });
		m_currentQuad->bottomLeft.setTextureIndex(textureIndex);

		m_currentQuad->bottomRight.setPosition(transform * m_localCoords[1]);
		m_currentQuad->bottomRight.setColor(color);
		m_currentQuad->bottomRight.setUV({ 1.0f, 0.0f });
		m_currentQuad->bottomRight.setTextureIndex(textureIndex);

		m_currentQuad->topRight.setPosition(transform * m_localCoords[2]);
		m_currentQuad->topRight.setColor(color);
		m_currentQuad->topRight.setUV({ 1.0f , 1.0f});
		m_currentQuad->topRight.setTextureIndex(textureIndex);

		m_currentQuad->topLeft.setPosition(transform * m_localCoords[3]);
		m_currentQuad->topLeft.setColor(color);
		m_currentQuad->topLeft.setUV({ 0.0f, 1.0f });
		m_currentQuad->topLeft.setTextureIndex(textureIndex);

		m_currentQuad++;

		m_vertexCount += 4;
		m_indexCount += 6;
	}

	void Renderer2D::drawQuad(const Texture& texture, const Color& color, const glm::mat4& transform, const glm::vec4& uv)
	{
		if (m_vertexCount >= MAX_VERTEX_COUNT || m_textureUnitIndex >= MAX_TEXTURE_UNITS) {
			end();
			flush();
			begin();
		}

		if (m_textureUnitMap.find(texture) == m_textureUnitMap.end()) {
			m_textureUnitMap[texture] = m_textureUnitIndex++;
		}

		float textureIndex = m_textureUnitMap.at(texture);

		m_currentQuad->bottomLeft.setPosition(transform * m_localCoords[0]);
		m_currentQuad->bottomLeft.setColor(color);
		m_currentQuad->bottomLeft.setUV({ uv.x, uv.y });
		m_currentQuad->bottomLeft.setTextureIndex(textureIndex);

		m_currentQuad->bottomRight.setPosition(transform * m_localCoords[1]);
		m_currentQuad->bottomRight.setColor(color);
		m_currentQuad->bottomRight.setUV({ uv.x + uv.z, uv.y });
		m_currentQuad->bottomRight.setTextureIndex(textureIndex);

		m_currentQuad->topRight.setPosition(transform * m_localCoords[2]);
		m_currentQuad->topRight.setColor(color);
		m_currentQuad->topRight.setUV({ uv.x + uv.z, uv.y + uv.w });
		m_currentQuad->topRight.setTextureIndex(textureIndex);

		m_currentQuad->topLeft.setPosition(transform * m_localCoords[3]);
		m_currentQuad->topLeft.setColor(color);
		m_currentQuad->topLeft.setUV({ uv.x, uv.y + uv.w });
		m_currentQuad->topLeft.setTextureIndex(textureIndex);

		m_currentQuad++;

		m_vertexCount += 4;
		m_indexCount += 6;
	}

	void Renderer2D::drawQuad(const Color& color, const glm::mat4& transform)
	{
		if (m_vertexCount >= MAX_VERTEX_COUNT) {
			end();
			flush();
			begin();
		}

		float textureIndex = 0.0f;

		m_currentQuad->bottomLeft.setPosition(transform * m_localCoords[0]);
		m_currentQuad->bottomLeft.setColor(color);
		m_currentQuad->bottomLeft.setUV({ 0.0f, 0.0f });
		m_currentQuad->bottomLeft.setTextureIndex(textureIndex);

		m_currentQuad->bottomRight.setPosition(transform * m_localCoords[1]);
		m_currentQuad->bottomRight.setColor(color);
		m_currentQuad->bottomRight.setUV({ 1.0f, 0.0f });
		m_currentQuad->bottomRight.setTextureIndex(textureIndex);

		m_currentQuad->topRight.setPosition(transform * m_localCoords[2]);
		m_currentQuad->topRight.setColor(color);
		m_currentQuad->topRight.setUV({ 1.0f, 1.0f });
		m_currentQuad->topRight.setTextureIndex(textureIndex);

		m_currentQuad->topLeft.setPosition(transform * m_localCoords[3]);
		m_currentQuad->topLeft.setColor(color);
		m_currentQuad->topLeft.setUV({ 0.0f, 1.0f });
		m_currentQuad->topLeft.setTextureIndex(textureIndex);

		m_currentQuad++;

		m_vertexCount += 4;
		m_indexCount += 6;
	}

	void Renderer2D::drawQuad(const Texture& texture, const Color& color, const glm::vec3& position, float rotationInDegrees, const glm::vec2& size, const glm::vec4& uv)
	{
		if (m_vertexCount >= MAX_VERTEX_COUNT || m_textureUnitIndex >= MAX_TEXTURE_UNITS) {
			end();
			flush();
			begin();
		}

		if (m_textureUnitMap.find(texture) == m_textureUnitMap.end()) {
			m_textureUnitMap[texture] = m_textureUnitIndex++;
		}

		float textureIndex = m_textureUnitMap.at(texture);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(position));
		model = glm::rotate(model, glm::radians(-rotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(size / 2.0f, 1.0f));

		m_currentQuad->bottomLeft.setPosition(model * m_localCoords[0]);
		m_currentQuad->bottomLeft.setColor(color);
		m_currentQuad->bottomLeft.setUV({ uv.x, uv.y });
		m_currentQuad->bottomLeft.setTextureIndex(textureIndex);

		m_currentQuad->bottomRight.setPosition(model * m_localCoords[1]);
		m_currentQuad->bottomRight.setColor(color);
		m_currentQuad->bottomRight.setUV({ uv.x + uv.z, uv.y });
		m_currentQuad->bottomRight.setTextureIndex(textureIndex);

		m_currentQuad->topRight.setPosition(model * m_localCoords[2]);
		m_currentQuad->topRight.setColor(color);
		m_currentQuad->topRight.setUV({ uv.x + uv.z, uv.y + uv.w });
		m_currentQuad->topRight.setTextureIndex(textureIndex);

		m_currentQuad->topLeft.setPosition(model * m_localCoords[3]);
		m_currentQuad->topLeft.setColor(color);
		m_currentQuad->topLeft.setUV({ uv.x, uv.y + uv.w });
		m_currentQuad->topLeft.setTextureIndex(textureIndex);

		m_currentQuad++;

		m_vertexCount += 4;
		m_indexCount += 6;
	}

	void Renderer2D::drawQuad(const Texture& texture, const glm::vec3& position, float rotationInDegrees, const glm::vec2& size, const glm::vec4& uv)
	{
		drawQuad(texture, { 1.0f, 1.0f, 1.0f, 1.0f }, position, rotationInDegrees, size, uv);
	}

	void Renderer2D::drawQuad(const Color& color, const glm::vec3& position, float rotationInDegrees, const glm::vec2& size)
	{
		if (m_vertexCount >= MAX_VERTEX_COUNT) {
			end();
			flush();
			begin();
		}

		float textureIndex = 0.0f;

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(position));
		model = glm::rotate(model, glm::radians(-rotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(size / 2.0f, 1.0f));

		m_currentQuad->bottomLeft.setPosition(model * m_localCoords[0]);
		m_currentQuad->bottomLeft.setColor(color);
		m_currentQuad->bottomLeft.setUV({ 0.0f, 0.0f });
		m_currentQuad->bottomLeft.setTextureIndex(textureIndex);

		m_currentQuad->bottomRight.setPosition(model * m_localCoords[1]);
		m_currentQuad->bottomRight.setColor(color);
		m_currentQuad->bottomRight.setUV({ 1.0f, 0.0f });
		m_currentQuad->bottomRight.setTextureIndex(textureIndex);

		m_currentQuad->topRight.setPosition(model * m_localCoords[2]);
		m_currentQuad->topRight.setColor(color);
		m_currentQuad->topRight.setUV({ 1.0f, 1.0f });
		m_currentQuad->topRight.setTextureIndex(textureIndex);

		m_currentQuad->topLeft.setPosition(model * m_localCoords[3]);
		m_currentQuad->topLeft.setColor(color);
		m_currentQuad->topLeft.setUV({ 0.0f, 1.0f });
		m_currentQuad->topLeft.setTextureIndex(textureIndex);

		m_currentQuad++;

		m_vertexCount += 4;
		m_indexCount += 6;
	}

	void Renderer2D::end()
	{
		std::sort(m_quads, m_quads + (m_currentQuad - m_quads), sortByDepth);
		m_vbo.bind();
		m_vbo.uploadData(m_vertexCount * sizeof(Vertex), m_quads);
	}

	void Renderer2D::flush()
	{
		for (const auto& [id, unit] : m_textureUnitMap) {
			GLEC(glBindTextureUnit(GLuint(unit), id));
		}

		m_vao.bind();
		GLEC(glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr));
	}

	bool Renderer2D::sortByDepth(Quad a, Quad b)
	{
		return a.bottomLeft.position.z > b.bottomLeft.position.z;
	}
}
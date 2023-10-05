#include "VertexArray.h"
#include "ECheck/ECheck.h"
#include "Vertex.h"
#include "Utils/Dark.h"

namespace Dark
{
	VertexArray::VertexArray()
		: m_vaoID(0), m_idGenerated(false)
	{
		//empty
	}

	VertexArray::~VertexArray()
	{
		if (m_idGenerated)
			error("Did not delete Vertex Array!", DARK_FILE, DARK_LINE);
	}

	void VertexArray::gen()
	{
		if (!m_idGenerated) {
			GLEC(glGenVertexArrays(1, &m_vaoID));
			m_idGenerated = true;
		}
	}

	void VertexArray::del()
	{
		if (m_idGenerated) {
			GLEC(glDeleteVertexArrays(1, &m_vaoID));
			m_idGenerated = false;
		}
	}

	void VertexArray::build(VertexBuffer& vertexBuffer, IndexBuffer& indexBuffer)
	{
		bind();
		vertexBuffer.bind();

		int offset = 0;

		for (int i = 0; i < m_attributes.size(); i++) {
			GLEC(glEnableVertexAttribArray(i));
			GLEC(glVertexAttribPointer(i, m_attributes[i].size, m_attributes[i].type, m_attributes[i].normalized, sizeof(Vertex), (const void*)offset));
			offset += m_attributes[i].size * m_attributes[i].originalTypeSize;
		}

		indexBuffer.bind();
	}

	void VertexArray::bind() const
	{
		GLEC(glBindVertexArray(m_vaoID));
	}

	void VertexArray::unbind() const
	{
		GLEC(glBindVertexArray(0));
	}
}
#include "IndexBuffer.h"
#include "ECheck/ECheck.h"
#include "Utils/Dark.h"

#include <vector>

namespace Dark
{
	IndexBuffer::IndexBuffer()
		: m_iboID(0), m_count(0), m_idGenerated(false)
	{
		//empty
	}

	IndexBuffer::~IndexBuffer()
	{
		if (m_idGenerated)
			error("Did not delete Index Buffer!", DARK_FILE, DARK_LINE);
	}

	void IndexBuffer::gen()
	{
		if (!m_idGenerated) {
			GLEC(glGenBuffers(1, &m_iboID));
			m_idGenerated = true;
		}
	}

	void IndexBuffer::del()
	{
		if (m_idGenerated) {
			GLEC(glDeleteBuffers(1, &m_iboID));
			m_idGenerated = false;
		}
	}

	void IndexBuffer::uploadDataCount(GLsizei count)
	{
		if (count % 6 != 0)
			error("[INDEX BUFFER]: \"count\" must be multiple of 6!", DARK_FILE, DARK_LINE);

		m_count = count;

		std::vector<unsigned int> indices(count);

		int offset = 0;

		for (int i = 0; i < indices.size(); i+=6) {
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;
			offset += 4;
		}

		GLEC(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
	}

	void IndexBuffer::bind() const
	{
		GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID));
	}

	void IndexBuffer::unbind() const
	{
		GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
}
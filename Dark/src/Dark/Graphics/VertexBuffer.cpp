#include "VertexBuffer.h"
#include "ECheck/ECheck.h"
#include "Utils/Dark.h"

namespace Dark
{
	VertexBuffer::VertexBuffer()
		: m_vboID(0), m_idGenerated(false)
	{
		//empty
	}

	VertexBuffer::~VertexBuffer()
	{
		if (m_idGenerated)
			error("Did not delete Vertex Buffer!", DARK_FILE, DARK_LINE);
	}

	void VertexBuffer::gen()
	{
		if (!m_idGenerated) {
			GLEC(glGenBuffers(1, &m_vboID));
			m_idGenerated = true;
		}
	}

	void VertexBuffer::del()
	{
		if (m_idGenerated) {
			GLEC(glDeleteBuffers(1, &m_vboID));
			m_idGenerated = false;
		}
	}

	void VertexBuffer::allocMemory(GLsizeiptr size, GLenum usage)
	{
		GLEC(glBufferData(GL_ARRAY_BUFFER, size, nullptr, usage));
	}

	void VertexBuffer::uploadData(GLsizeiptr size, const void* data)
	{
		GLEC(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
	}

	void VertexBuffer::bind() const
	{
		GLEC(glBindBuffer(GL_ARRAY_BUFFER, m_vboID));
	}

	void VertexBuffer::unbind() const
	{
		GLEC(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
}
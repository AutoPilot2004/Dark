#pragma once
#include <GL/glew.h>

namespace Dark
{
	class VertexBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

		void gen();
		void del();

		void allocMemory(GLsizeiptr size, GLenum usage);
		void uploadData(GLsizeiptr size, const void* data);

		void bind() const;
		void unbind() const;

	private:
		GLuint m_vboID;

		bool m_idGenerated;
	};
}
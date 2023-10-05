#pragma once
#include <GL/glew.h>

namespace Dark
{
	class IndexBuffer
	{
	public:
		IndexBuffer();
		~IndexBuffer();

		void gen();
		void del();

		void uploadDataCount(GLsizei count);

		void bind() const;
		void unbind() const;

		//Getters
		inline GLsizei getCount() const { return m_count; }

	private:
		GLuint m_iboID;

		GLsizei m_count;

		bool m_idGenerated;
	};
}
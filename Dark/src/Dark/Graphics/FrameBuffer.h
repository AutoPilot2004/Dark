#pragma once
#include <GL/glew.h>

namespace Dark
{
	class FrameBuffer
	{
	public:
		FrameBuffer();
		~FrameBuffer();

		void gen();
		void del();

		void resize(int width, int height);

		void bind() const;
		void unbind() const;

		//Getters
		inline GLuint getTex() const { return m_texID; }

	private:
		GLuint m_fboID;
		GLuint m_texID;
		GLuint m_rboID;

		bool m_idGenerated;
	};
}
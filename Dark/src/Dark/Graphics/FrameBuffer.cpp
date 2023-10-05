#include "FrameBuffer.h"
#include "ECheck/ECheck.h"

namespace Dark
{
	FrameBuffer::FrameBuffer()
		: m_fboID(0), m_texID(0), m_rboID(0), m_idGenerated(false)
	{
		//empty
	}

	FrameBuffer::~FrameBuffer()
	{
		if (m_idGenerated)
			error("Did not delete framebuffer and/or its attachments!", DARK_FILE, DARK_LINE);
	}

	void FrameBuffer::gen()
	{
		//Generate buffers
		if (!m_idGenerated) {
			GLEC(glGenFramebuffers(1, &m_fboID));
			GLEC(glGenTextures(1, &m_texID));
			GLEC(glGenRenderbuffers(1, &m_rboID));

			//Attach attachments to the fbo
			GLEC(glBindFramebuffer(GL_FRAMEBUFFER, m_fboID));

			GLEC(glBindTexture(GL_TEXTURE_2D, m_texID));
			GLEC(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr));
			GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GLEC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texID, 0));

			GLEC(glBindRenderbuffer(GL_RENDERBUFFER, m_rboID));
			GLEC(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1, 1));
			GLEC(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboID));

			//Check if framebuffer is complete
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				error("FrameBuffer is not complete!", DARK_FILE, DARK_LINE);

			//Unbind buffers
			GLEC(glBindFramebuffer(GL_FRAMEBUFFER, 0));
			GLEC(glBindTexture(GL_TEXTURE_2D, 0));
			GLEC(glBindRenderbuffer(GL_RENDERBUFFER, 0));

			m_idGenerated = true;
		}
	}

	void FrameBuffer::del()
	{
		if (m_idGenerated) {
			GLEC(glDeleteFramebuffers(1, &m_fboID));
			GLEC(glDeleteTextures(1, &m_texID));
			GLEC(glDeleteRenderbuffers(1, &m_rboID));
			m_idGenerated = false;
		}
	}

	void FrameBuffer::resize(int width, int height)
	{
		GLEC(glBindTexture(GL_TEXTURE_2D, m_texID));
		GLEC(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr));
		GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLEC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texID, 0));

		GLEC(glBindRenderbuffer(GL_RENDERBUFFER, m_rboID));
		GLEC(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
		GLEC(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboID));
	}

	void FrameBuffer::bind() const
	{
		GLEC(glBindFramebuffer(GL_FRAMEBUFFER, m_fboID));
	}

	void FrameBuffer::unbind() const
	{
		GLEC(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
}
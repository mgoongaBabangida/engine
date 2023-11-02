#include "stdafx.h"
#include "colorFBO.h"
#include <iostream>

eColorFBO::~eColorFBO()
{
}

bool eColorFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight, bool multisample, bool mask_attachment)
{
	m_width = WindowWidth;
	m_height = WindowHeight;
	// Framebuffers
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	if (!multisample)
	{
		//Generate texture ID and load texture data 
		glGenTextures(1, &m_texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, Width(), Height(), 0, attachment_type, GL_UNSIGNED_BYTE, NULL);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, WindowWidth, WindowHeight); //GL_RGBA16F for  hdr
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Create a color attachment texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

		if (mask_attachment)
		{
			glGenTextures(1, &m_texture_mask);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texture_mask);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WindowWidth, WindowHeight, 0, GL_RED, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_texture_mask, 0);

			unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			glDrawBuffers(2, attachments);
		}
	}
	else
		CreateMultisampleColorAttachement(WindowWidth, WindowHeight);

	//GLuint rbo;
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	if (!multisample)
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WindowWidth, WindowHeight); // Use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
	}
	else
	{
		//glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight); // Use a single renderbuffer object for both a depth AND stencil buffer.
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, WindowWidth, WindowHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
	}
	// Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void eColorFBO::CreateMultisampleColorAttachement(unsigned int WindowWidth, unsigned int WindowHeight)
{
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA16F, WindowWidth, WindowHeight); //GL_RGBA16F for hdr
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_rbo);

	GLuint rbo_mask;
	glGenRenderbuffers(1, &rbo_mask);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_mask);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RED, WindowWidth, WindowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, rbo_mask);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}

void eColorFBO::ResolveToFBO(eColorFBO* other)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, other->m_fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, this->m_width, this->m_height, 0, 0, other->m_width, other->m_height,
		GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, this->m_width, this->m_height, 0, 0, other->m_width, other->m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void eColorFBO::ResolveToScreen(unsigned int WindowWidth, unsigned int WindowHeight)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glBlitFramebuffer(0, 0, this->m_width, this->m_height, 0, 0, WindowWidth, WindowHeight,
		GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void SimpleColorFBO::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void SimpleColorFBO::BindForReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void SimpleColorFBO::BindForReadingMask(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_texture_mask);
}

Texture SimpleColorFBO::GetTexture()
{
	return Texture(m_texture, m_width, m_height, 4);
}

Texture SimpleColorFBO::GetTextureMask()
{
	return Texture(m_texture_mask, m_width, m_height, 1);
}

bool SimpleColorFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight, bool multisample, bool mask_attachment)
{
	m_width = WindowWidth;
	m_height = WindowHeight;

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WindowWidth, WindowHeight, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

	if (mask_attachment)
	{
		glGenTextures(1, &m_texture_mask);
		glBindTexture(GL_TEXTURE_2D, m_texture_mask);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WindowWidth, WindowHeight, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_texture_mask, 0);

		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Framebuffer not complete!" << std::endl;
	return true;
}

SimpleColorFBO::~SimpleColorFBO()
{
	if (m_fbo != 0) {
		glDeleteFramebuffers(1, &m_fbo);
		glDeleteTextures(1, &m_texture);
	}
}

CubemapFBO::~CubemapFBO()
{
	m_textureBuffer.freeTexture();
	if (m_fbo != 0) {
		glDeleteFramebuffers(1, &m_fbo);
		glDeleteTextures(1, &m_texture);
	}
}

bool CubemapFBO::Init(unsigned int _size)
{
	m_width = _size;
	m_height = _size;
	glGenFramebuffers(1, &m_fbo);
	glGenRenderbuffers(1, &m_rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _size, _size);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
	
	m_textureBuffer.makeCubemap(_size);
	m_texture = m_textureBuffer.id;

	for (unsigned int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_texture, 0);
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Cube Framebuffer not complete!" << std::endl;
	return true;
}

void CubemapFBO::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
}

void CubemapFBO::BindForReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
}

Texture CubemapFBO::GetTexture()
{
	return Texture(m_texture, m_width, m_height, m_textureBuffer.mChannels); //!
}

#include "stdafx.h"
#include "colorFBO.h"

eColorFBO::eColorFBO()
{
}

eColorFBO::~eColorFBO()
{
	if (m_fbo != 0) {
		glDeleteFramebuffers(1, &m_fbo);
	}
}

bool eColorFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight, bool multisample)
{
	m_width = WindowWidth;
	m_height = WindowHeight;
	// Framebuffers
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	// What enum to use?
	GLenum attachment_type;
	attachment_type = GL_RGB;

	if (!multisample)
	{
		//Generate texture ID and load texture data 
		glGenTextures(1, &m_texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, Width(), Height(), 0, attachment_type, GL_UNSIGNED_BYTE, NULL);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, WindowWidth, WindowHeight);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Create a color attachment texture

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
	}
	else
		CreateMultisampleColorAttachement(WindowWidth, WindowHeight);

	//GLuint rbo;
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	if (!multisample)
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight); // Use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
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
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, WindowWidth, WindowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo); 
																																												  // Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}

void eColorFBO::ResolveToFBO(eColorFBO* other)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, other->m_fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glBlitFramebuffer(0, 0, this->m_width, this->m_height, 0, 0, other->m_width, other->m_height,
		GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
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

void eColorFBO::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void eColorFBO::BindForReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

Texture eColorFBO::GetTexture()
{
	return Texture(m_texture, m_width, m_height);
}

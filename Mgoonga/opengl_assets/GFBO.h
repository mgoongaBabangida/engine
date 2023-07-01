#ifndef GFBO_H 
#define GFBO_H

#include "Texture.h"

//--------------------------------------------------------------------------
class eGFBO
{
public:
	eGFBO() = default;
	~eGFBO();

	bool		Init(unsigned int WindowWidth, unsigned int WindowHeight, bool multisample = false);
	void		BindForWriting();

	void		BindForReading0(GLenum TextureUnit);
	void		BindForReading1(GLenum TextureUnit);
	void		BindForReading2(GLenum TextureUnit);
	
	GLuint		Width()		  { return  m_width; }
	GLuint		Height()	  { return  m_height; }

	Texture		GetTexture0() { return Texture(gPosition, m_width, m_height);	}
	Texture		GetTexture1() { return Texture(gNormal, m_width, m_height);		}
	Texture		GetTexture2() { return Texture(gAlbedoSpec, m_width, m_height); }

	GLuint		ID()		  { return m_fbo; }
	glm::ivec2 Size()		{ return { m_width , m_height }; }

private:
	GLuint	m_fbo;
	GLuint	m_rbo;
	GLuint	m_width;
	GLuint	m_height;
	GLuint	gPosition, gNormal, gAlbedoSpec;
	GLuint	rboDepth;
};

#endif /* GFBO_H */

#ifndef COLORFBO_H 
#define COLORFBO_H

#include "Texture.h"

class eColorFBO
{
public:
	eColorFBO();
	~eColorFBO();
	bool		Init(unsigned int WindowWidth, unsigned int WindowHeight, bool multisample = false);
	void		ResolveToFBO(eColorFBO* other);
	void		ResolveToScreen(unsigned int WindowWidth, unsigned int WindowHeight);
	void		BindForWriting();
	void		BindForReading(GLenum TextureUnit);
	Texture		GetTexture();
	GLuint		Width()		{ return  m_width;	}
	GLuint		Height()	{ return  m_height; }
private:
	void		CreateMultisampleColorAttachement(unsigned int WindowWidth, unsigned int WindowHeight);
	GLuint		m_fbo;
	GLuint		m_rbo;
	GLuint		m_texture;
	GLuint		m_width;
	GLuint		m_height;
};


#endif /* COLORFBO_H */



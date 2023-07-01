#ifndef COLORFBO_H 
#define COLORFBO_H

#include "Texture.h"

//-------------------------------------------------------------------------------------
class SimpleColorFBO
{
public:
	SimpleColorFBO() = default;
	virtual ~SimpleColorFBO();

	void		BindForWriting();
	void		BindForReading(GLenum TextureUnit);

	Texture		GetTexture();
	GLuint  ID() { return m_fbo; }
	GLuint		Width() { return  m_width; }
	GLuint		Height() { return  m_height; }
	glm::ivec2 Size() { return { m_width , m_height }; }

	virtual bool		Init(unsigned int WindowWidth, unsigned int WindowHeight, bool multisample = false);
protected:
	GLuint		m_fbo;
	GLuint		m_texture;
	GLuint		m_width;
	GLuint		m_height;
};

//-------------------------------------------------------------------------------------
class eColorFBO : public SimpleColorFBO
{
public:
	eColorFBO() = default;
	~eColorFBO();

	virtual bool		Init(unsigned int WindowWidth, unsigned int WindowHeight, bool multisample = false) override;

	void		ResolveToFBO(eColorFBO* other);
	void		ResolveToScreen(unsigned int WindowWidth, unsigned int WindowHeight);

protected:
	void		CreateMultisampleColorAttachement(unsigned int WindowWidth, unsigned int WindowHeight);

	GLuint		m_rbo;
};


#endif /* COLORFBO_H */



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
	void		BindForReadingMask(GLenum TextureUnit);

	Texture		GetTexture();
	Texture		GetTextureMask();

	GLuint			ID()		 { return m_fbo; }
	GLuint			Width()  { return  m_width; }
	GLuint			Height() { return  m_height; }
	glm::ivec2	Size()	 { return { m_width , m_height }; }

	virtual bool		Init(unsigned int WindowWidth, unsigned int WindowHeight, bool multisample = false, bool mask_attachment = false);
protected:
	GLuint		m_fbo;
	GLuint		m_texture;
	GLuint		m_width;
	GLuint		m_height;
	GLuint		m_texture_mask;
};

//-------------------------------------------------------------------------------------
class eColorFBO : public SimpleColorFBO
{
public:
	eColorFBO() = default;
	~eColorFBO();
	eColorFBO(const eColorFBO&) = delete;
	const eColorFBO& operator=(const eColorFBO&) = delete;

	eColorFBO(eColorFBO&&);

	virtual bool		Init(unsigned int WindowWidth, unsigned int WindowHeight, bool multisample = false, bool mask_attachment = false) override;

	void		ResolveToFBO(eColorFBO* other);
	void		ResolveToScreen(unsigned int WindowWidth, unsigned int WindowHeight);

protected:
	void		CreateMultisampleColorAttachement(unsigned int WindowWidth, unsigned int WindowHeight);

	GLuint		m_rbo;
	GLuint		m_rbo_mask;
};

//-------------------------------------------------------------------------------------
class CubemapFBO
{
public:
	~CubemapFBO();

	 bool		Init(unsigned int _size);

	 void		BindForWriting();
	 void		BindForReading(GLenum TextureUnit);

	 Texture		GetTexture();
	 GLuint			ID() { return m_fbo; }
	 GLuint			RboID() { return m_rbo; }
	 GLuint			Width() { return  m_width; }
	 GLuint			Height() { return  m_height; }
	 glm::ivec2 Size() { return { m_width , m_height }; }

protected:
	GLuint		m_fbo;
	GLuint		m_texture;
	GLuint		m_width;
	GLuint		m_height;
	GLuint		m_rbo;

	Texture		m_textureBuffer;
};

#endif /* COLORFBO_H */



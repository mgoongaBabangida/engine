#pragma once

#include <glew-2.1.0\include\GL\glew.h>
#include <glm\glm\glm.hpp>

#include "opengl_assets.h"

#include <base/base.h>

#include <type_traits>
#include <assert.h>
#include <set>

enum TColor { WHITE, BLACK, BLUE, PINK, YELLOW, GREY };

//---------------------------------------------------------------------------------------------
struct DLL_OPENGL_ASSETS Texture
{
public:
	GLuint				  m_id;
	std::string			m_type;
	std::string			m_path;
	
	int32_t				m_width;
	int32_t				m_height;

	int32_t				m_channels	=	1;
	int32_t				m_layers		= 1;
	int32_t				m_num_rows	= 1;

public:
	Texture();
	Texture(const TextureInfo&);
	Texture(GLuint Width, GLuint Height, int32_t TextureChannels);
	Texture(GLuint ID, GLuint TextureWidth, GLuint TextureHeight, int32_t TextureChannels = 1);

	~Texture();

	Texture(const Texture& t) { this->operator=(t);}
	Texture& operator=(const Texture& t) 
	{ 
		m_id				= t.m_id,
		m_width	= t.m_width,
		m_height	= t.m_height,
		m_type			= t.m_type,
		m_path			= t.m_path;
		m_channels		= t.m_channels;
		m_layers = t.m_layers;
		m_num_rows	= t.m_num_rows;
		return *this; 
	}

	static GLuint		GetDefaultTextureId();
	static void 		SetEmptyTextureId(GLuint);
	static GLuint		GetLightIconTextureId();
	static void 		SetLightIconTextureId(GLuint);
	static GLuint		GetEmptyTextureId();

	static Texture	GetTexture1x1(TColor color);
	static bool			freeTexture(unsigned int _id);

	void setNumRows(GLuint nrows) { m_num_rows = nrows; }
	void freeTexture();

	uint8_t*	getPixelBuffer(GLenum _target = GL_TEXTURE_2D, GLenum format = GL_RGBA, GLenum _type = GL_UNSIGNED_BYTE) const;
	bool			saveToFile(const std::string &path, GLenum _target = GL_TEXTURE_2D, GLenum format = GL_RGBA, GLenum _type = GL_UNSIGNED_BYTE);

	bool loadTextureFromFile(const std::string& path, GLenum format = GL_RGBA, GLenum wrap = GL_CLAMP_TO_EDGE);
	bool loadCubemap(std::vector<std::string> faces);
	bool loadHdr(const std::string& _path);
	bool loadTexture2DArray(std::vector<std::string> _paths, GLenum format = GL_RGBA);

	bool generatePerlin(GLuint Width, GLuint Height, bool periodic);

	bool makeCubemap(size_t size, bool _mipmap = false, GLenum _format = GL_RGB, GLenum _internal_format = GL_RGB16F, GLenum _type = GL_FLOAT);
	bool makeDepthTexture();
	bool makeDepthTextureArray(int32_t _layers);
	bool makeDepthCubeMap();
	bool makeRandom1DTexture(unsigned int _size);
	bool makeImage(GLuint Width, GLuint Height);

	template<class GLtype>
	bool TextureFromBuffer(const GLtype* buffer, GLuint Width, GLuint Height, GLuint format= GL_RGBA, GLenum wrap = GL_CLAMP_TO_EDGE, GLenum minFilter = GL_LINEAR)
	{
		GLenum type ;
		if (std::is_same_v<GLtype, GLfloat> == true)
			type = GL_FLOAT;
		else if (std::is_same_v<GLtype, GLubyte> == true)
			type = GL_UNSIGNED_BYTE;
		//else
			//static assert

		this->m_width = Width;
		this->m_height = Height;
		if (m_id == GetDefaultTextureId())
			_genTexture();
		glBindTexture(GL_TEXTURE_2D, m_id);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexStorage2D(GL_TEXTURE_2D, 1, format, this->m_width, this->m_height);
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, type, buffer);
		if(format == GL_RGBA)
			m_channels = 4;
		else if(format == GL_RED)
			m_channels = 1;
		else if(format == GL_RGB)
			m_channels = 3;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

protected:
	bool _loadTexture1x1(TColor color);
	void _genTexture();

	static unsigned int						textures_in_use;
	static std::set<unsigned int> indexes_in_use;
	static GLuint									mg_default_texture_id;
	static GLuint									mg_empty_texture_id;
	static GLuint									mg_light_icon_texture_id;
	static bool										m_use_mipmaps;
};




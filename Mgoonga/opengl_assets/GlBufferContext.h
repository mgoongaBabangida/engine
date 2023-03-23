#pragma once

#include "colorFBO.h"
#include "ShadowMapFbo.h"
#include "GFBO.h"

enum class eBuffer
{
	BUFFER_DEFAULT,
	BUFFER_SHADOW,
	BUFFER_BRIGHT_FILTER,
	BUFFER_GAUSSIAN_ONE,
	BUFFER_GAUSSIAN_TWO,
	BUFFER_REFLECTION,
	BUFFER_REFRACTION,
	BUFFER_SCREEN,
	BUFFER_MTS,
	BUFFER_DEFFERED,
	BUFFER_DEFFERED1,
	BUFFER_DEFFERED2,
	BUFFER_SQUERE,
};

//---------------------------------------------------------------------------
class eGlBufferContext
{
public:
	eGlBufferContext()								{}
	eGlBufferContext(const eGlBufferContext&)		= delete;
	eGlBufferContext& operator=(eGlBufferContext&)	= delete;
	
	void BufferInit(eBuffer, unsigned int, unsigned int, bool _param = false);
	void EnableWrittingBuffer(eBuffer);
	void EnableReadingBuffer(eBuffer, GLenum slot);
	Texture GetTexture(eBuffer);
	void ResolveMtsToScreen() { mtsFBO.ResolveToFBO(&screenFBO); }
	void BlitFromTo(eBuffer, eBuffer, GLenum bit);

	GLuint GetId(eBuffer);
	glm::ivec2 eGlBufferContext::GetSize(eBuffer _buffer);

	static eGlBufferContext& GetInstance() 
	{
		static eGlBufferContext  instance;
		return instance;
	}

private:
	eColorFBO		  defaultFBO;
	ShadowMapFBO	depthFBO;
  eColorFBO		  screenFBO;
  eColorFBO		  mtsFBO;
  eColorFBO		  reflectionFBO;
  eColorFBO		  refractionFBO;
  eColorFBO		  brightFilterFBO;
  eColorFBO		  gausian1FBO;
  eColorFBO		  gausian2FBO;
  eColorFBO		  squereFBO;
	eGFBO			    gFBO;
};

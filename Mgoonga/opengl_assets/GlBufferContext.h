#pragma once

#include "colorFBO.h"
#include "ShadowMapFbo.h"
#include "GFBO.h"

enum class eBuffer
{
	BUFFER_DEFAULT,
	BUFFER_SHADOW_DIR,
	BUFFER_SHADOW_CUBE_MAP,
	BUFFER_SHADOW_CSM,
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
	BUFFER_SSAO,
	BUFFER_SSAO_BLUR,
	BUFFER_IBL_CUBEMAP,
	BUFFER_IBL_CUBEMAP_IRR
};

//---------------------------------------------------------------------------
class eGlBufferContext
{
public:
	eGlBufferContext()								{}
	eGlBufferContext(const eGlBufferContext&)		= delete;
	eGlBufferContext& operator=(eGlBufferContext&)	= delete;
	
	void BufferInit(eBuffer, unsigned int, unsigned int);
	void EnableWrittingBuffer(eBuffer);
	void EnableReadingBuffer(eBuffer, GLenum slot);

	Texture GetTexture(eBuffer);
	GLuint GetRboID(eBuffer _buffer);

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
	eColorFBO		   defaultFBO;
	ShadowMapFBO	 depthDirFBO;
	ShadowMapFBO	 depthCubeFBO;
	ShadowMapFBO	 depthCSMFBO;
  eColorFBO		   screenFBO;
  eColorFBO		   mtsFBO;
  eColorFBO		   reflectionFBO;
  eColorFBO		   refractionFBO;
  eColorFBO		   brightFilterFBO;
  eColorFBO		   gausian1FBO;
  eColorFBO		   gausian2FBO;
  eColorFBO		   squereFBO;
	eGFBO			     gFBO;
	SimpleColorFBO ssaoFBO;
	SimpleColorFBO ssaoBlurFBO;
	CubemapFBO		 iblCubemapFBO;
	CubemapFBO		 iblCubemapIrrFBO;
};

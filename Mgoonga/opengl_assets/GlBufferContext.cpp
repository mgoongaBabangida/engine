#include "stdafx.h"
#include "GlBufferContext.h"

void eGlBufferContext::BufferInit(eBuffer _buffer, unsigned int _width, unsigned int _height)
{
	switch (_buffer)
	{
		case eBuffer::BUFFER_DEFAULT:							defaultFBO.Init(_width, _height);							break;
		case eBuffer::BUFFER_SHADOW_DIR:					depthDirFBO.Init(_width, _height, false);			break;
		case eBuffer::BUFFER_SHADOW_CUBE_MAP:			depthCubeFBO.Init(_width, _height, true);			break;
		case eBuffer::BUFFER_SHADOW_CSM:					depthCSMFBO.InitCSM(_width, _height, 5);			break; //@todo layers
		case eBuffer::BUFFER_BRIGHT_FILTER:				brightFilterFBO.Init(_width, _height);				break;
		case eBuffer::BUFFER_GAUSSIAN_ONE:				gausian1FBO.Init(_width, _height);						break;
		case eBuffer::BUFFER_GAUSSIAN_TWO:				gausian2FBO.Init(_width, _height);						break;
		case eBuffer::BUFFER_REFLECTION:					reflectionFBO.Init(_width, _height);					break;
		case eBuffer::BUFFER_REFRACTION:					refractionFBO.Init(_width, _height);					break;
		case eBuffer::BUFFER_SSR:									ssrFBO.Init(_width, _height);									break;
		case eBuffer::BUFFER_SSR_BLUR:						ssrblurFBO.Init(_width, _height);							break;
		case eBuffer::BUFFER_SCREEN:							screenFBO.Init(_width, _height, false, true);	break;
		case eBuffer::BUFFER_MTS:									mtsFBO.Init(_width, _height, true);						break;
		case eBuffer::BUFFER_DEFFERED:						gFBO.Init(_width, _height);										break;
		case eBuffer::BUFFER_SQUERE:							squereFBO.Init(_width, _height);							break; // assert width == height
		case eBuffer::BUFFER_SSAO:								ssaoFBO.Init(_width, _height);								break;
		case eBuffer::BUFFER_SSAO_BLUR:						ssaoBlurFBO.Init(_width, _height);						break;
		case eBuffer::BUFFER_IBL_CUBEMAP:					iblCubemapFBO.Init(_width);										break;
		case eBuffer::BUFFER_IBL_CUBEMAP_IRR:			iblCubemapIrrFBO.Init(_width);								break;
		case eBuffer::BUFFER_ENVIRONMENT_CUBEMAP: environmentCubemapFBO.Init(_width);						break;
		case eBuffer::BUFFER_BLOOM:								bloomFBO.Init(_width, _height, 5);						break; //@todo magic number
		case eBuffer::BUFFER_SCREEN_WITH_SSR:			screenSsrFBO.Init(_width, _height);						break;
	}
}

GLuint eGlBufferContext::BufferCustomInit(const std::string& _name, unsigned int _width, unsigned int _height, bool _multisample, bool _mask_attachement)
{
	if (auto it = customBuffers.find(_name); it != customBuffers.end())
		return it->second.ID();

	eColorFBO buffer;
	buffer.Init(_width, _height);
	GLuint id = buffer.ID();
	customBuffers.insert({ _name, std::move(buffer) });
	return id;
}

void eGlBufferContext::EnableWrittingBuffer(eBuffer _buffer)
{
	switch(_buffer)
	{
		case eBuffer::BUFFER_DEFAULT:							defaultFBO.BindForWriting();						break;
		case eBuffer::BUFFER_SHADOW_DIR:					depthDirFBO.BindForWriting();						break;
		case eBuffer::BUFFER_SHADOW_CUBE_MAP:			depthCubeFBO.BindForWriting();					break;
		case eBuffer::BUFFER_SHADOW_CSM:					depthCSMFBO.BindForWriting();						break;
		case eBuffer::BUFFER_BRIGHT_FILTER:				brightFilterFBO.BindForWriting();				break;
		case eBuffer::BUFFER_GAUSSIAN_ONE:				gausian1FBO.BindForWriting();						break;
		case eBuffer::BUFFER_GAUSSIAN_TWO:				gausian2FBO.BindForWriting();						break;
		case eBuffer::BUFFER_REFLECTION:					reflectionFBO.BindForWriting();					break;
		case eBuffer::BUFFER_REFRACTION:					refractionFBO.BindForWriting();					break;
		case eBuffer::BUFFER_SCREEN:							screenFBO.BindForWriting();							break;
		case eBuffer::BUFFER_MTS:									mtsFBO.BindForWriting();								break;
		case eBuffer::BUFFER_DEFFERED:						gFBO.BindForWriting();									break;
		case eBuffer::BUFFER_SQUERE:							squereFBO.BindForWriting();							break;
		case eBuffer::BUFFER_SSAO:								ssaoFBO.BindForWriting();								break;
		case eBuffer::BUFFER_SSAO_BLUR:						ssaoBlurFBO.BindForWriting();						break;
		case eBuffer::BUFFER_IBL_CUBEMAP:					iblCubemapFBO.BindForWriting();					break;
		case eBuffer::BUFFER_IBL_CUBEMAP_IRR:			iblCubemapIrrFBO.BindForWriting();			break;
		case eBuffer::BUFFER_ENVIRONMENT_CUBEMAP: environmentCubemapFBO.BindForWriting(); break;
		case eBuffer::BUFFER_BLOOM:								bloomFBO.BindForWriting();							break;
		case eBuffer::BUFFER_SSR:									ssrFBO.BindForWriting();								break;
		case eBuffer::BUFFER_SSR_BLUR:						ssrblurFBO.BindForWriting();						break;
		case eBuffer::BUFFER_SCREEN_WITH_SSR:			screenSsrFBO.BindForWriting();					break;
	}
}

void eGlBufferContext::EnableReadingBuffer(eBuffer _buffer, GLenum _slot)
{
	switch (_buffer)
	{
		case eBuffer::BUFFER_DEFAULT:							defaultFBO.BindForReading(_slot);						 break;
		case eBuffer::BUFFER_SHADOW_DIR:					depthDirFBO.BindForReading(_slot);					 break;
		case eBuffer::BUFFER_SHADOW_CUBE_MAP:			depthCubeFBO.BindForReading(_slot);					 break;
		case eBuffer::BUFFER_SHADOW_CSM:					depthCSMFBO.BindForReading(_slot);					 break;
		case eBuffer::BUFFER_BRIGHT_FILTER:				brightFilterFBO.BindForReading(_slot);			 break;
		case eBuffer::BUFFER_GAUSSIAN_ONE:				gausian1FBO.BindForReading(_slot);					 break;
		case eBuffer::BUFFER_GAUSSIAN_TWO:				gausian2FBO.BindForReading(_slot);					 break;
		case eBuffer::BUFFER_REFLECTION:					reflectionFBO.BindForReading(_slot);				 break;
		case eBuffer::BUFFER_REFRACTION:					refractionFBO.BindForReading(_slot);				 break;
		case eBuffer::BUFFER_SCREEN:							screenFBO.BindForReading(_slot);						 break;
		case eBuffer::BUFFER_MTS:									mtsFBO.BindForReading(_slot);								 break;
		case eBuffer::BUFFER_DEFFERED:						gFBO.BindForReading0(_slot);								 break;
		case eBuffer::BUFFER_DEFFERED1:						gFBO.BindForReading1(_slot);								 break;
		case eBuffer::BUFFER_DEFFERED2:						gFBO.BindForReading2(_slot);								 break;
		case eBuffer::BUFFER_SQUERE:							squereFBO.BindForReading(_slot);						 break;
		case eBuffer::BUFFER_SSAO:								ssaoFBO.BindForReading(_slot);							 break;
		case eBuffer::BUFFER_SSAO_BLUR:						ssaoBlurFBO.BindForReading(_slot);					 break;
		case eBuffer::BUFFER_IBL_CUBEMAP:					iblCubemapFBO.BindForReading(_slot);				 break;
		case eBuffer::BUFFER_IBL_CUBEMAP_IRR:			iblCubemapIrrFBO.BindForReading(_slot);			 break;
		case eBuffer::BUFFER_ENVIRONMENT_CUBEMAP: environmentCubemapFBO.BindForReading(_slot); break;
		case eBuffer::BUFFER_BLOOM:								bloomFBO.BindForReading(_slot);							 break;
		case eBuffer::BUFFER_SSR:									ssrFBO.BindForReading(_slot);								 break;
		case eBuffer::BUFFER_SSR_BLUR:						ssrblurFBO.BindForReading(_slot);						 break;
		case eBuffer::BUFFER_SCREEN_MASK:					screenFBO.BindForReadingMask(_slot);				 break;
		case eBuffer::BUFFER_SCREEN_WITH_SSR:			screenSsrFBO.BindForReading(_slot);					 break;
	}
}

void eGlBufferContext::EnableCustomWrittingBuffer(const std::string& _name)
{
	if (auto it = customBuffers.find(_name); it != customBuffers.end())
		it->second.BindForWriting();
}

void eGlBufferContext::EnableCustomReadingBuffer(const std::string& _name, GLenum slot)
{
	if (auto it = customBuffers.find(_name); it != customBuffers.end())
		it->second.BindForReading(slot);
}

GLuint eGlBufferContext::GetRboID(eBuffer _buffer)
{
	switch (_buffer)
	{
		case eBuffer::BUFFER_IBL_CUBEMAP:				return iblCubemapFBO.RboID();
			//@todo all other
			throw;
			return -1;
	}
}

Texture eGlBufferContext::GetTexture(eBuffer _buffer)
{
	switch (_buffer)
	{
		case eBuffer::BUFFER_DEFAULT:							return defaultFBO.GetTexture();
		case eBuffer::BUFFER_SHADOW_DIR:					return depthDirFBO.GetTexture();
		case eBuffer::BUFFER_SHADOW_CUBE_MAP:			return depthCubeFBO.GetTexture();
		case eBuffer::BUFFER_SHADOW_CSM:					return depthCSMFBO.GetTexture();
		case eBuffer::BUFFER_BRIGHT_FILTER:				return brightFilterFBO.GetTexture();
		case eBuffer::BUFFER_GAUSSIAN_ONE:				return gausian1FBO.GetTexture();
		case eBuffer::BUFFER_GAUSSIAN_TWO:				return gausian2FBO.GetTexture();
		case eBuffer::BUFFER_REFLECTION:					return reflectionFBO.GetTexture();
		case eBuffer::BUFFER_REFRACTION:					return refractionFBO.GetTexture();
		case eBuffer::BUFFER_SCREEN:							return screenFBO.GetTexture();
		case eBuffer::BUFFER_MTS:									return mtsFBO.GetTexture();
		case eBuffer::BUFFER_DEFFERED:						return gFBO.GetTexture0();
		case eBuffer::BUFFER_DEFFERED1:						return gFBO.GetTexture1();
		case eBuffer::BUFFER_DEFFERED2:						return gFBO.GetTexture2();
		case eBuffer::BUFFER_SQUERE:							return squereFBO.GetTexture();
		case eBuffer::BUFFER_SSAO:								return ssaoFBO.GetTexture();
		case eBuffer::BUFFER_SSAO_BLUR:						return ssaoBlurFBO.GetTexture();
		case eBuffer::BUFFER_IBL_CUBEMAP:					return iblCubemapFBO.GetTexture();
		case eBuffer::BUFFER_IBL_CUBEMAP_IRR:			return iblCubemapIrrFBO.GetTexture();
		case eBuffer::BUFFER_ENVIRONMENT_CUBEMAP: return environmentCubemapFBO.GetTexture();
		case eBuffer::BUFFER_BLOOM:								return bloomFBO.GetTexture();
		case eBuffer::BUFFER_SSR:									return ssrFBO.GetTexture();
		case eBuffer::BUFFER_SSR_BLUR:						return ssrblurFBO.GetTexture();
		case eBuffer::BUFFER_SCREEN_MASK:					return screenFBO.GetTextureMask();
		case eBuffer::BUFFER_SCREEN_WITH_SSR:			return screenSsrFBO.GetTexture();
	}
	return Texture();/*?*/
}

Texture eGlBufferContext::GetTexture(const std::string& _name)
{
	if (auto it = customBuffers.find(_name); it != customBuffers.end())
		return it->second.GetTexture();
	else
		return Texture();/*?*/
}

//----------------------------------------------------------------
void eGlBufferContext::BlitFromTo(eBuffer _from, eBuffer _to, GLenum bit)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, GetId(_from));
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GetId(_to));
	glBlitFramebuffer(0, 0, GetSize(_from).x, GetSize(_from).y, 0, 0,
													GetSize(_to).x, GetSize(_to).y, bit, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//----------------------------------------------------------------
GLuint eGlBufferContext::GetId(eBuffer _buffer)
{
	switch (_buffer)
	{
	case eBuffer::BUFFER_DEFAULT:							return defaultFBO.ID();
	case eBuffer::BUFFER_SHADOW_DIR:					return depthDirFBO.ID();
	case eBuffer::BUFFER_SHADOW_CUBE_MAP:			return depthCubeFBO.ID();
	case eBuffer::BUFFER_SHADOW_CSM:					return depthCSMFBO.ID();
	case eBuffer::BUFFER_BRIGHT_FILTER:				return brightFilterFBO.ID();
	case eBuffer::BUFFER_GAUSSIAN_ONE:				return gausian1FBO.ID();
	case eBuffer::BUFFER_GAUSSIAN_TWO:				return gausian2FBO.ID();
	case eBuffer::BUFFER_REFLECTION:					return reflectionFBO.ID();
	case eBuffer::BUFFER_REFRACTION:					return refractionFBO.ID();
	case eBuffer::BUFFER_SCREEN:							return screenFBO.ID();
	case eBuffer::BUFFER_MTS:									return mtsFBO.ID();
	case eBuffer::BUFFER_DEFFERED:						return gFBO.ID();
	case eBuffer::BUFFER_DEFFERED1:						return gFBO.ID();
	case eBuffer::BUFFER_DEFFERED2:						return gFBO.ID();
	case eBuffer::BUFFER_SQUERE:							return squereFBO.ID();
	case eBuffer::BUFFER_SSAO:								return ssaoFBO.ID();
	case eBuffer::BUFFER_SSAO_BLUR:						return ssaoBlurFBO.ID();
	case eBuffer::BUFFER_IBL_CUBEMAP:					return iblCubemapFBO.ID();
	case eBuffer::BUFFER_IBL_CUBEMAP_IRR:			return iblCubemapIrrFBO.ID();
	case eBuffer::BUFFER_ENVIRONMENT_CUBEMAP: return environmentCubemapFBO.ID();
	case eBuffer::BUFFER_BLOOM:								return bloomFBO.ID();
	case eBuffer::BUFFER_SSR:									return ssrFBO.ID();
	case eBuffer::BUFFER_SSR_BLUR:						return ssrblurFBO.ID();
	case eBuffer::BUFFER_SCREEN_WITH_SSR:			return screenSsrFBO.ID();
	case eBuffer::BUFFER_SCREEN_MASK:					return screenFBO.ID();
	}
	assert("buffer is not finished");
	throw;
}


//----------------------------------------------------------------
glm::ivec2 eGlBufferContext::GetSize(eBuffer _buffer)
{
	switch (_buffer)
	{
	case eBuffer::BUFFER_DEFAULT:							return defaultFBO.Size();
	case eBuffer::BUFFER_SHADOW_DIR:					return depthDirFBO.Size();
	case eBuffer::BUFFER_SHADOW_CUBE_MAP:			return depthCubeFBO.Size();
	case eBuffer::BUFFER_SHADOW_CSM:					return depthCSMFBO.Size();
	case eBuffer::BUFFER_BRIGHT_FILTER:				return brightFilterFBO.Size();
	case eBuffer::BUFFER_GAUSSIAN_ONE:				return gausian1FBO.Size();
	case eBuffer::BUFFER_GAUSSIAN_TWO:				return gausian2FBO.Size();
	case eBuffer::BUFFER_REFLECTION:					return reflectionFBO.Size();
	case eBuffer::BUFFER_REFRACTION:					return refractionFBO.Size();
	case eBuffer::BUFFER_SCREEN:							return screenFBO.Size();
	case eBuffer::BUFFER_MTS:									return mtsFBO.Size();
	case eBuffer::BUFFER_DEFFERED:						return gFBO.Size();
	case eBuffer::BUFFER_DEFFERED1:						return gFBO.Size();
	case eBuffer::BUFFER_DEFFERED2:						return gFBO.Size();
	case eBuffer::BUFFER_SQUERE:							return squereFBO.Size();
	case eBuffer::BUFFER_SSAO:								return ssaoFBO.Size();
	case eBuffer::BUFFER_SSAO_BLUR:						return ssaoBlurFBO.Size();
	case eBuffer::BUFFER_IBL_CUBEMAP:					return iblCubemapFBO.Size();
	case eBuffer::BUFFER_IBL_CUBEMAP_IRR:			return iblCubemapIrrFBO.Size();
	case eBuffer::BUFFER_ENVIRONMENT_CUBEMAP: return environmentCubemapFBO.Size();
	case eBuffer::BUFFER_BLOOM:								return bloomFBO.Size();
	case eBuffer::BUFFER_SSR:									return ssrFBO.Size();
	case eBuffer::BUFFER_SSR_BLUR:						return ssrblurFBO.Size();
	case eBuffer::BUFFER_SCREEN_WITH_SSR:			return screenSsrFBO.Size();
	case eBuffer::BUFFER_SCREEN_MASK:					return screenFBO.Size();
	}
	assert("buffer is not finished");
	throw;
}
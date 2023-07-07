#include "stdafx.h"
#include "GlBufferContext.h"

void eGlBufferContext::BufferInit(eBuffer _buffer, unsigned int _width, unsigned int _height, bool _param)
{
	switch (_buffer)
	{
		case eBuffer::BUFFER_DEFAULT:					defaultFBO.Init(_width, _height);				break;
		case eBuffer::BUFFER_SHADOW:					depthFBO.Init(_width, _height, _param);	break;
		case eBuffer::BUFFER_BRIGHT_FILTER:		brightFilterFBO.Init(_width, _height);	break;
		case eBuffer::BUFFER_GAUSSIAN_ONE:		gausian1FBO.Init(_width, _height);			break;
		case eBuffer::BUFFER_GAUSSIAN_TWO:		gausian2FBO.Init(_width, _height);			break;
		case eBuffer::BUFFER_REFLECTION:			reflectionFBO.Init(_width, _height);		break;
		case eBuffer::BUFFER_REFRACTION:			refractionFBO.Init(_width, _height);		break;
		case eBuffer::BUFFER_SCREEN:					screenFBO.Init(_width, _height);				break;
		case eBuffer::BUFFER_MTS:							mtsFBO.Init(_width, _height, true);			break;
		case eBuffer::BUFFER_DEFFERED:				gFBO.Init(_width, _height);							break;
		case eBuffer::BUFFER_SQUERE:					squereFBO.Init(_width, _height);				break; // assert width == height
		case eBuffer::BUFFER_SSAO:						ssaoFBO.Init(_width, _height);					break;
		case eBuffer::BUFFER_SSAO_BLUR:				ssaoBlurFBO.Init(_width, _height);			break;
		case eBuffer::BUFFER_IBL_CUBEMAP:			iblCubemapFBO.Init(_width);							break;
		case eBuffer::BUFFER_IBL_CUBEMAP_IRR: iblCubemapIrrFBO.Init(_width);					break;
	}
}

void eGlBufferContext::EnableWrittingBuffer(eBuffer _buffer)
{
	switch(_buffer)
	{
		case eBuffer::BUFFER_DEFAULT:						defaultFBO.BindForWriting();				break;
		case eBuffer::BUFFER_SHADOW:						depthFBO.BindForWriting();					break;
		case eBuffer::BUFFER_BRIGHT_FILTER:			brightFilterFBO.BindForWriting();		break;
		case eBuffer::BUFFER_GAUSSIAN_ONE:			gausian1FBO.BindForWriting();				break;
		case eBuffer::BUFFER_GAUSSIAN_TWO:			gausian2FBO.BindForWriting();				break;
		case eBuffer::BUFFER_REFLECTION:				reflectionFBO.BindForWriting();			break;
		case eBuffer::BUFFER_REFRACTION:				refractionFBO.BindForWriting();			break;
		case eBuffer::BUFFER_SCREEN:						screenFBO.BindForWriting();					break;
		case eBuffer::BUFFER_MTS:								mtsFBO.BindForWriting();						break;
		case eBuffer::BUFFER_DEFFERED:					gFBO.BindForWriting();							break;
		case eBuffer::BUFFER_SQUERE:						squereFBO.BindForWriting();					break;
		case eBuffer::BUFFER_SSAO:							ssaoFBO.BindForWriting();						break;
		case eBuffer::BUFFER_SSAO_BLUR:					ssaoBlurFBO.BindForWriting();				break;
		case eBuffer::BUFFER_IBL_CUBEMAP:				iblCubemapFBO.BindForWriting();			break;
		case eBuffer::BUFFER_IBL_CUBEMAP_IRR:		iblCubemapIrrFBO.BindForWriting();	break;
	}
}

void eGlBufferContext::EnableReadingBuffer(eBuffer _buffer, GLenum _slot)
{
	switch (_buffer)
	{
		case eBuffer::BUFFER_DEFAULT:						defaultFBO.BindForReading(_slot);					break;
		case eBuffer::BUFFER_SHADOW:						depthFBO.BindForReading(_slot);						break;
		case eBuffer::BUFFER_BRIGHT_FILTER:			brightFilterFBO.BindForReading(_slot);		break;
		case eBuffer::BUFFER_GAUSSIAN_ONE:			gausian1FBO.BindForReading(_slot);				break;
		case eBuffer::BUFFER_GAUSSIAN_TWO:			gausian2FBO.BindForReading(_slot);				break;
		case eBuffer::BUFFER_REFLECTION:				reflectionFBO.BindForReading(_slot);			break;
		case eBuffer::BUFFER_REFRACTION:				refractionFBO.BindForReading(_slot);			break;
		case eBuffer::BUFFER_SCREEN:						screenFBO.BindForReading(_slot);					break;
		case eBuffer::BUFFER_MTS:								mtsFBO.BindForReading(_slot);							break;
		case eBuffer::BUFFER_DEFFERED:					gFBO.BindForReading0(_slot);							break;
		case eBuffer::BUFFER_DEFFERED1:					gFBO.BindForReading1(_slot);							break;
		case eBuffer::BUFFER_DEFFERED2:					gFBO.BindForReading2(_slot);							break;
		case eBuffer::BUFFER_SQUERE:						squereFBO.BindForReading(_slot);					break;
		case eBuffer::BUFFER_SSAO:							ssaoFBO.BindForReading(_slot);						break;
		case eBuffer::BUFFER_SSAO_BLUR:					ssaoBlurFBO.BindForReading(_slot);				break;
		case eBuffer::BUFFER_IBL_CUBEMAP:				iblCubemapFBO.BindForReading(_slot);			break;
		case eBuffer::BUFFER_IBL_CUBEMAP_IRR:		iblCubemapIrrFBO.BindForReading(_slot);		break;
	}
}

Texture eGlBufferContext::GetTexture(eBuffer _buffer)
{
	switch (_buffer)
	{
		case eBuffer::BUFFER_DEFAULT:						return defaultFBO.GetTexture();
		case eBuffer::BUFFER_SHADOW:						return depthFBO.GetTexture();
		case eBuffer::BUFFER_BRIGHT_FILTER:			return brightFilterFBO.GetTexture();
		case eBuffer::BUFFER_GAUSSIAN_ONE:			return gausian1FBO.GetTexture();
		case eBuffer::BUFFER_GAUSSIAN_TWO:			return gausian2FBO.GetTexture();
		case eBuffer::BUFFER_REFLECTION:				return reflectionFBO.GetTexture();
		case eBuffer::BUFFER_REFRACTION:				return refractionFBO.GetTexture();
		case eBuffer::BUFFER_SCREEN:						return screenFBO.GetTexture();
		case eBuffer::BUFFER_MTS:								return mtsFBO.GetTexture();
		case eBuffer::BUFFER_DEFFERED:					return gFBO.GetTexture0();
		case eBuffer::BUFFER_DEFFERED1:					return gFBO.GetTexture1();
		case eBuffer::BUFFER_DEFFERED2:					return gFBO.GetTexture2();
		case eBuffer::BUFFER_SQUERE:						return squereFBO.GetTexture();
		case eBuffer::BUFFER_SSAO:							return ssaoFBO.GetTexture();
		case eBuffer::BUFFER_SSAO_BLUR:					return ssaoBlurFBO.GetTexture();
		case eBuffer::BUFFER_IBL_CUBEMAP:				return iblCubemapFBO.GetTexture();
		case eBuffer::BUFFER_IBL_CUBEMAP_IRR:		return iblCubemapIrrFBO.GetTexture();
	}
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
	case eBuffer::BUFFER_DEFAULT:						return defaultFBO.ID();
	case eBuffer::BUFFER_SHADOW:						return depthFBO.ID();
	case eBuffer::BUFFER_BRIGHT_FILTER:			return brightFilterFBO.ID();
	case eBuffer::BUFFER_GAUSSIAN_ONE:			return gausian1FBO.ID();
	case eBuffer::BUFFER_GAUSSIAN_TWO:			return gausian2FBO.ID();
	case eBuffer::BUFFER_REFLECTION:				return reflectionFBO.ID();
	case eBuffer::BUFFER_REFRACTION:				return refractionFBO.ID();
	case eBuffer::BUFFER_SCREEN:						return screenFBO.ID();
	case eBuffer::BUFFER_MTS:								return mtsFBO.ID();
	case eBuffer::BUFFER_DEFFERED:					return gFBO.ID();
	case eBuffer::BUFFER_DEFFERED1:					return gFBO.ID();
	case eBuffer::BUFFER_DEFFERED2:					return gFBO.ID();
	case eBuffer::BUFFER_SQUERE:						return squereFBO.ID();
	case eBuffer::BUFFER_SSAO:							return ssaoFBO.ID();
	case eBuffer::BUFFER_SSAO_BLUR:					return ssaoBlurFBO.ID();
	case eBuffer::BUFFER_IBL_CUBEMAP:				return iblCubemapFBO.ID();
	case eBuffer::BUFFER_IBL_CUBEMAP_IRR:		return iblCubemapIrrFBO.ID();
	}
	assert("buffer is not finished");
}


//----------------------------------------------------------------
glm::ivec2 eGlBufferContext::GetSize(eBuffer _buffer)
{
	switch (_buffer)
	{
	case eBuffer::BUFFER_DEFAULT:						return defaultFBO.Size();
	case eBuffer::BUFFER_SHADOW:						return depthFBO.Size();
	case eBuffer::BUFFER_BRIGHT_FILTER:			return brightFilterFBO.Size();
	case eBuffer::BUFFER_GAUSSIAN_ONE:			return gausian1FBO.Size();
	case eBuffer::BUFFER_GAUSSIAN_TWO:			return gausian2FBO.Size();
	case eBuffer::BUFFER_REFLECTION:				return reflectionFBO.Size();
	case eBuffer::BUFFER_REFRACTION:				return refractionFBO.Size();
	case eBuffer::BUFFER_SCREEN:						return screenFBO.Size();
	case eBuffer::BUFFER_MTS:								return mtsFBO.Size();
	case eBuffer::BUFFER_DEFFERED:					return gFBO.Size();
	case eBuffer::BUFFER_DEFFERED1:					return gFBO.Size();
	case eBuffer::BUFFER_DEFFERED2:					return gFBO.Size();
	case eBuffer::BUFFER_SQUERE:						return squereFBO.Size();
	case eBuffer::BUFFER_SSAO:							return ssaoFBO.Size();
	case eBuffer::BUFFER_SSAO_BLUR:					return ssaoBlurFBO.Size();
	case eBuffer::BUFFER_IBL_CUBEMAP:				return iblCubemapFBO.Size();
	case eBuffer::BUFFER_IBL_CUBEMAP_IRR:		return iblCubemapIrrFBO.Size();
	}
	assert("buffer is not finished");
}
#include "GlBufferContext.h"

void eGlBufferContext::BufferInit(eBuffer _buffer, unsigned int _width, unsigned int _height)
{
	switch (_buffer)
	{
		case eBuffer::BUFFER_DEFAULT:												break;
		case eBuffer::BUFFER_SHADOW:		depthFBO.Init(_width, _height);			break;
		case eBuffer::BUFFER_BRIGHT_FILTER: brightFilterFBO.Init(_width, _height);	break;
		case eBuffer::BUFFER_GAUSSIAN_ONE:	gausian1FBO.Init(_width, _height);		break;
		case eBuffer::BUFFER_GAUSSIAN_TWO:	gausian2FBO.Init(_width, _height);		break;
		case eBuffer::BUFFER_REFLECTION:	reflectionFBO.Init(_width, _height);	break;
		case eBuffer::BUFFER_REFRACTION:	refractionFBO.Init(_width, _height);	break;
		case eBuffer::BUFFER_SCREEN:		screenFBO.Init(_width, _height);		break;
		case eBuffer::BUFFER_MTS:			mtsFBO.Init(_width, _height, true);		break;
	}
}

void eGlBufferContext::EnableWrittingBuffer(eBuffer _buffer)
{
	switch(_buffer)
	{
		case eBuffer::BUFFER_DEFAULT:		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);	break;
		case eBuffer::BUFFER_SHADOW:		depthFBO.BindForWriting();					break;
		case eBuffer::BUFFER_BRIGHT_FILTER: brightFilterFBO.BindForWriting();			break;
		case eBuffer::BUFFER_GAUSSIAN_ONE:	gausian1FBO.BindForWriting();				break;
		case eBuffer::BUFFER_GAUSSIAN_TWO:	gausian2FBO.BindForWriting();				break;
		case eBuffer::BUFFER_REFLECTION:	reflectionFBO.BindForWriting();				break;
		case eBuffer::BUFFER_REFRACTION:	refractionFBO.BindForWriting();				break;
		case eBuffer::BUFFER_SCREEN:		screenFBO.BindForWriting();					break;
		case eBuffer::BUFFER_MTS:			mtsFBO.BindForWriting();					break;
	}
}

void eGlBufferContext::EnableReadingBuffer(eBuffer _buffer, GLenum _slot)
{
	switch (_buffer)
	{
	case eBuffer::BUFFER_DEFAULT:		/*?*/										break;
	case eBuffer::BUFFER_SHADOW:		depthFBO.BindForReading(_slot);				break;
	case eBuffer::BUFFER_BRIGHT_FILTER: brightFilterFBO.BindForReading(_slot);		break;
	case eBuffer::BUFFER_GAUSSIAN_ONE:	gausian1FBO.BindForReading(_slot);			break;
	case eBuffer::BUFFER_GAUSSIAN_TWO:	gausian2FBO.BindForReading(_slot);			break;
	case eBuffer::BUFFER_REFLECTION:	reflectionFBO.BindForReading(_slot);		break;
	case eBuffer::BUFFER_REFRACTION:	refractionFBO.BindForReading(_slot);		break;
	case eBuffer::BUFFER_SCREEN:		screenFBO.BindForReading(_slot);			break;
	case eBuffer::BUFFER_MTS:			mtsFBO.BindForReading(_slot);				break;
	}
}

Texture eGlBufferContext::GetTexture(eBuffer _buffer)
{
	switch (_buffer)
	{
	case eBuffer::BUFFER_DEFAULT:		return Texture();/*?*/
	case eBuffer::BUFFER_SHADOW:		return depthFBO.GetTexture();
	case eBuffer::BUFFER_BRIGHT_FILTER: return brightFilterFBO.GetTexture();
	case eBuffer::BUFFER_GAUSSIAN_ONE:	return gausian1FBO.GetTexture();
	case eBuffer::BUFFER_GAUSSIAN_TWO:	return gausian2FBO.GetTexture();
	case eBuffer::BUFFER_REFLECTION:	return reflectionFBO.GetTexture();	
	case eBuffer::BUFFER_REFRACTION:	return refractionFBO.GetTexture();	
	case eBuffer::BUFFER_SCREEN:		return screenFBO.GetTexture();
	case eBuffer::BUFFER_MTS:			return mtsFBO.GetTexture();	
	}
	return Texture();/*?*/
}

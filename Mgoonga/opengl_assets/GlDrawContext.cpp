#include "stdafx.h"
#include <glew-2.1.0\include\GL\glew.h>
#include "GlDrawContext.h"

//-------------------------------------------------------------------------------------------------------------------------
void eGlDrawContext::DrawElements(GLenum _mode, GLsizei _count, GLenum _type, const void* _indices, const std::string& _tag)
{
	glDrawElements(_mode, _count, _type, _indices);
	if (m_debuging)
	{
		_Debug(_tag);
	}
}

//-------------------------------------------------------------------------------------------------------------------------
void eGlDrawContext::DrawElementsInstanced(GLenum _mode, GLsizei _count, GLenum _type, const void* _indices, GLsizei _instances, const std::string& _tag)
{
	glDrawElementsInstanced(_mode, _count, _type, _indices, _instances);
	if (m_debuging)
	{
		_Debug(_tag);
	}
}

//-------------------------------------------------------------------------------------------------------------------------
void eGlDrawContext::DrawArrays(GLenum _mode, GLint _first, GLsizei _count, const std::string& _tag)
{
	glDrawArrays(_mode, _first, _count);
	if (m_debuging)
	{
		_Debug(_tag);
	}
}
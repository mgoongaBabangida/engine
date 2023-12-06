#include "stdafx.h"
#include "Shader.h"

#include <fstream>

//@todo make one function installShaders//

//------------------------------------------------------------------------------------
void Shader::installShaders(const char* VertexShaderName, const char* FragmentShaderName, bool _transformFeedback)
{
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const char* adapter[1];
	std::string temp = readShaderCode(VertexShaderName);
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, NULL);
	temp = readShaderCode(FragmentShaderName);
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, NULL);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);
	id = glCreateProgram();

	if (checkShaderStatus(vertexShaderID) | checkShaderStatus(fragmentShaderID))
	{
		glAttachShader(id, vertexShaderID);
		glAttachShader(id, fragmentShaderID);

		if (_transformFeedback)
		{
			const GLchar* Varyings[3];
			Varyings[0] = "Position";
			Varyings[1] = "Velocity";
			Varyings[2] = "StartTime";
			glTransformFeedbackVaryings(id, 3, Varyings, GL_SEPARATE_ATTRIBS);
		}

		glLinkProgram(id);
	}

	if (!checkProgramStatus())
		return;
}

//--------------------------------------------------------------------------------------
void Shader::installShaders(const char* VertexShaderName, const char* FragmentShaderName, const char* GeometryShaderName, bool _transformFeedback)
{
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

	const char* adapter[1];
	std::string temp = readShaderCode(VertexShaderName);
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, NULL);
	temp = readShaderCode(FragmentShaderName);
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, NULL);
	temp = readShaderCode(GeometryShaderName);
	adapter[0] = temp.c_str();
	glShaderSource(geometryShaderID, 1, adapter, NULL);

	glCompileShader(vertexShaderID);
	glCompileShader(geometryShaderID);
	glCompileShader(fragmentShaderID);

	id = glCreateProgram();

	if (checkShaderStatus(vertexShaderID) | checkShaderStatus(fragmentShaderID) | checkShaderStatus(geometryShaderID))
	{
		glAttachShader(id, vertexShaderID);
		glAttachShader(id, geometryShaderID);
		glAttachShader(id, fragmentShaderID);

		if (_transformFeedback)
		{
			const GLchar* Varyings[3];
			Varyings[0] = "Position";
			Varyings[1] = "Velocity";
			Varyings[2] = "StartTime";
			glTransformFeedbackVaryings(id, 3, Varyings, GL_SEPARATE_ATTRIBS);
		}
		glLinkProgram(id);
	}

	if (!checkProgramStatus())
		return;
}

//--------------------------------------------------------------------
void	Shader::installShaders(const char* _vertexShaderName,
														 const char* _fragmentShaderName,
														 const char* _tessellation1ShaderName,
														 const char* _tessellation2ShaderName)
{
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	tessellation1ShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	tessellation2ShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);

	const char* adapter[1];
	std::string temp = readShaderCode(_vertexShaderName);
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, NULL);
	temp = readShaderCode(_fragmentShaderName);
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, NULL);
	temp = readShaderCode(_tessellation1ShaderName);
	adapter[0] = temp.c_str();
	glShaderSource(tessellation1ShaderID, 1, adapter, NULL);
	temp = readShaderCode(_tessellation2ShaderName);
	adapter[0] = temp.c_str();
	glShaderSource(tessellation2ShaderID, 1, adapter, NULL);

	glCompileShader(vertexShaderID);
	glCompileShader(tessellation1ShaderID);
	glCompileShader(tessellation2ShaderID);
	glCompileShader(fragmentShaderID);
	id = glCreateProgram();

	if (checkShaderStatus(vertexShaderID)
		| checkShaderStatus(fragmentShaderID)
		| checkShaderStatus(tessellation1ShaderID)
		| checkShaderStatus(tessellation2ShaderID))
	{
		glAttachShader(id, vertexShaderID);
		glAttachShader(id, tessellation1ShaderID);
		glAttachShader(id, tessellation2ShaderID);
		glAttachShader(id, fragmentShaderID);
		glLinkProgram(id);
	}

	if (!checkProgramStatus())
		return;
}

//--------------------------------------------------------------------
bool Shader::checkShaderStatus(GLint shaderID)
{
	GLint compileStatus;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		GLint infologlength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infologlength);
		GLchar* buffer = new GLchar[infologlength];

		GLint bufferSize;
		glGetShaderInfoLog(shaderID, infologlength, &bufferSize, buffer);
		std::cout << buffer << std::endl;
		delete[] buffer;
	}
	return true;
}

//----------------------------------------------------------------
bool Shader::checkProgramStatus()
{
	GLint linkStatus;
	glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		GLint infologlength;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infologlength);
		GLchar* buffer = new GLchar[infologlength];

		GLint bufferSize;
		glGetProgramInfoLog(id, infologlength, &bufferSize, buffer);
		std::cout << buffer << std::endl;
		delete[] buffer;
	}
	return true;
}

//---------------------------------------------------------
std::string Shader::readShaderCode(const char * filename)
{

	std::ifstream meInput(filename);
	if (!meInput.good())
	{
		std::cout << "File failed to load..." << std::endl;
		exit(1);
	}
	return std::string(std::istreambuf_iterator<char>(meInput), std::istreambuf_iterator<char>());
}

//--------------------------------------------------
Shader::~Shader()
{
	glDeleteShader(vertexShaderID);
	glDeleteShader(geometryShaderID);
	glDeleteShader(tessellation1ShaderID);
	glDeleteShader(tessellation2ShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteProgram(id);
}

//---------------------------------------------------
void	Shader::GetUniformInfoFromShader()
{
	if (m_uniforms.empty())
	{
		GLint numuniforms = 0;
		glGetProgramInterfaceiv(id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numuniforms);

		std::cout << "Active uniforms " << id << std::endl;
		GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };
		for (int i = 0; i < numuniforms; ++i)
		{
			GLint results[4];
			glGetProgramResourceiv(id, GL_UNIFORM, i, 4, properties, 4, NULL, results);
			if (results[3] != -1)
				continue;
			GLint nameBufSize = results[0] + 1;
			char* name = new char[nameBufSize];
			glGetProgramResourceName(id, GL_UNIFORM, i, nameBufSize, NULL, name);
			m_uniforms.push_back(Uniform{ std::string(name),results[2],results[1],{} });
			delete[] name;
			std::cout << m_uniforms.back().name << " " << m_uniforms.back().type << " " << m_uniforms.back().location << std::endl;
		}
	}
}

//---------------------------------------------------
void	Shader::GetUniformDataFromShader()
{
	for (auto& uniform : m_uniforms)
	{
		switch (uniform.type)
		{
			case GL_BOOL:
			{
				GLint res;
				glGetUniformiv(id, uniform.location, &res);
				uniform.data = static_cast<bool>(res);
			}
			break;
			case GL_INT:
			{
				GLint res;
				glGetUniformiv(id, uniform.location, &res);
				uniform.data = static_cast<int32_t>(res);
			}
			break;
			case GL_UNSIGNED_INT:
			{
				GLuint res;
				glGetUniformuiv(id, uniform.location, &res);
				uniform.data = static_cast<size_t>(res);
			}
			break;
			case GL_FLOAT:
			{
				GLfloat res;
				glGetUniformfv(id,uniform.location,&res);
				uniform.data = static_cast<float>(res);
			}
			break;
			case GL_FLOAT_VEC2:
			{
				glm::vec2 res;
				glGetUniformfv(id, uniform.location, &res[0]);
				uniform.data = res;
			}
			break;
			case GL_FLOAT_VEC4:
			{
				glm::vec4 res;
				glGetUniformfv(id, uniform.location, &res[0]);
				uniform.data = res;
			}
			break;
			case GL_FLOAT_MAT2:
			{
				glm::mat2 res;
				glGetUniformfv(id, uniform.location, &res[0][0]);
				uniform.data = res;
			}
			break;
			case GL_FLOAT_MAT4:
			{
				glm::mat4 res;
				glGetUniformfv(id, uniform.location, &res[0][0]);
				uniform.data = res;
			}
			break;
			case GL_SAMPLER_2D:
			{
				GLint res; // just the location of texture slot not texture id
				glGetUniformiv(id, uniform.location, &res);
				uniform.data = static_cast<int32_t>(res);
			}
			break;
			case GL_SAMPLER_CUBE:
			{
				GLint res; // just the location of texture slot not texture id
				glGetUniformiv(id, uniform.location, &res);
				uniform.data = static_cast<int32_t>(res);
			}
			break;
			case GL_SAMPLER_2D_SHADOW:
			{
				GLint res; // just the location of texture slot not texture id
				glGetUniformiv(id, uniform.location, &res);
				uniform.data = static_cast<int32_t>(res);
			}
			break;
			case GL_SAMPLER_2D_ARRAY:
			{
				GLint res; // just the location of texture slot not texture id
				glGetUniformiv(id, uniform.location, &res);
				uniform.data = static_cast<int32_t>(res);
			}
			break;
			default:
			{
				std::cout << "there is not uniform handler for type " << uniform.type << " "<< uniform.name << std::endl;
			}
		}
	}
}

//---------------------------------------------------
bool Shader::SetUniformData(const std::string& _name, const UniformData& _data)
{
	auto it = std::find_if(m_uniforms.begin(), m_uniforms.end(), [_name](const Uniform& u) {return u.name == _name; });
	if (it != m_uniforms.end())
	{
		it->data = _data;
		_SetUniform(*it);
		return true;
	}
	else // try to set it anyway
	{
		if (const int32_t* pval = std::get_if<int32_t>(&_data))
			glUniform1i(glGetUniformLocation(id, _name.c_str()), *pval);
		else if (const size_t* pval = std::get_if<size_t>(&_data))
			glUniform1i(glGetUniformLocation(id, _name.c_str()), *pval);
		else if (const bool* pval = std::get_if<bool>(&_data))
			glUniform1i(glGetUniformLocation(id, _name.c_str()), *pval);
		else if (const float* pval = std::get_if<float>(&_data))
			glUniform1f(glGetUniformLocation(id, _name.c_str()), *pval);
		else if (const glm::vec4* pval = std::get_if<glm::vec4>(&_data))
			glUniform4f(glGetUniformLocation(id, _name.c_str()), (*pval)[0], (*pval)[1], (*pval)[2], (*pval)[3]);
		//@todo other types
	}
	return false;
}

//---------------------------------------------------
void Shader::_SetUniform(const Uniform& _uniform)
{
	glUseProgram(this->id);
	switch (_uniform.type)
	{
		case GL_BOOL:
		{
			bool data = std::get<bool>(_uniform.data);
			glUniform1i(_uniform.location, data);
		}
		break;
		case GL_INT:
		{
			if(const int32_t* pval =  std::get_if<int32_t>(&_uniform.data))
				glUniform1i(_uniform.location, *pval);
			else if(const size_t* pval = std::get_if<size_t>(&_uniform.data))
				glUniform1i(_uniform.location, *pval);
		}
		break;
		case GL_UNSIGNED_INT:
		{
			if (const int32_t* pval = std::get_if<int32_t>(&_uniform.data))
				glUniform1i(_uniform.location, *pval);
			else if (const size_t* pval = std::get_if<size_t>(&_uniform.data))
				glUniform1i(_uniform.location, *pval);
		}
		break;
		case GL_FLOAT:
		{
			float data = std::get<float>(_uniform.data);
			glUniform1f(_uniform.location, data);
		}
		break;
		case GL_FLOAT_VEC2:
		{
			glm::vec2 data = std::get<glm::vec2>(_uniform.data);
			glUniform2f(_uniform.location, data[0], data[1]);
		}
		break;
		case GL_FLOAT_VEC4:
		{
			glm::vec4 data = std::get<glm::vec4>(_uniform.data);
			//glUniform4f(_uniform.location, data[0], data[1], data[2], data[3]);
			glUniform4fv(_uniform.location, 1, &data[0]);
		}
		break;
		case GL_FLOAT_MAT2:
		{
			glm::mat2 data = std::get<glm::mat2>(_uniform.data);
			glUniformMatrix2fv(_uniform.location, 1, GL_FALSE, &data[0][0]);
		}
		break;
		case GL_FLOAT_MAT4:
		{
			glm::mat4 data = std::get<glm::mat4>(_uniform.data);
			glUniformMatrix4fv(_uniform.location, 1, GL_FALSE, &data[0][0]);
		}
		break;
		case GL_SAMPLER_2D:
		{

		}
		break;
		case GL_SAMPLER_CUBE:
		{

		}
		break;
		case GL_SAMPLER_2D_SHADOW:
		{

		}
		break;
		default:
		{
			std::cout << "there is not uniform handler for type " << _uniform.type << " " << _uniform.name << std::endl;
		}
	}
}		

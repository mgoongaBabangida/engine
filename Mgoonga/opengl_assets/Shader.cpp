#include "stdafx.h"
#include "Shader.h"

#include <fstream>

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
	if (uniforms.empty())
	{
		GLint numUniforms = 0;
		glGetProgramInterfaceiv(id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

		std::cout << "Active uniforms " << id << std::endl;
		GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };
		for (int i = 0; i < numUniforms; ++i)
		{
			GLint results[4];
			glGetProgramResourceiv(id, GL_UNIFORM, i, 4, properties, 4, NULL, results);
			if (results[3] != -1)
				continue;
			GLint nameBufSize = results[0] + 1;
			char* name = new char[nameBufSize];
			glGetProgramResourceName(id, GL_UNIFORM, i, nameBufSize, NULL, name);
			uniforms.push_back(Uniform{ std::string(name),results[2],results[1],{} });
			delete[] name;
			std::cout << uniforms.back().name << " " << uniforms.back().type << " " << uniforms.back().location << std::endl;
		}
	}
}

//---------------------------------------------------
void	Shader::GetUniformDataFromShader()
{
	for (auto& uniform : uniforms)
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
			case GL_FLOAT_VEC3:
			{
				glm::vec3 res;
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
			case GL_FLOAT_MAT3:
			{
				glm::mat3 res;
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
				std::cout << "there is not uniform handler for type " << uniform.type << std::endl;
			}
		}
	}
}

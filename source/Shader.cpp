#include "stdafx.h"
#include "Shader.h"

void Shader::installShaders(const char* VertexShaderName, const char* FragmentShaderName)
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
	ID = glCreateProgram();

	if (checkShaderStatus(vertexShaderID) | checkShaderStatus(fragmentShaderID))
	{
		glAttachShader(ID, vertexShaderID);
		glAttachShader(ID, fragmentShaderID);
		glLinkProgram(ID);
	}

	if (!checkProgramStatus())
		return;
}

void Shader::installShaders(const char* VertexShaderName, const char* FragmentShaderName, const char* GeometryShaderName)
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
	ID = glCreateProgram();

	if (checkShaderStatus(vertexShaderID) | checkShaderStatus(fragmentShaderID) | checkShaderStatus(geometryShaderID))
	{
		glAttachShader(ID, vertexShaderID);
		glAttachShader(ID, geometryShaderID);
		glAttachShader(ID, fragmentShaderID);
		glLinkProgram(ID);
	}

	if (!checkProgramStatus())
		return;
}

bool Shader::checkShaderStatus(GLint shaderID) {

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

bool Shader::checkProgramStatus() {
	GLint linkStatus;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		GLint infologlength;
		glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &infologlength);
		GLchar* buffer = new GLchar[infologlength];

		GLint bufferSize;
		glGetProgramInfoLog(ID, infologlength, &bufferSize, buffer);
		std::cout << buffer << std::endl;
		delete[] buffer;
	}
	return true;
}
std::string Shader::readShaderCode(const char * filename) {

	std::ifstream meInput(filename);
	if (!meInput.good())
	{
		std::cout << "File failed to load..." << std::endl;
		exit(1);
	}
	return std::string(std::istreambuf_iterator<char>(meInput), std::istreambuf_iterator<char>());
}

Shader::~Shader()
{
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteProgram(ID);
}

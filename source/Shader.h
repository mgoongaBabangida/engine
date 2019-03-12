#pragma once

#include <glew-2.0.0\include\GL\glew.h>
#include <string>

struct Shader
{
	GLuint ID;
	GLuint vertexShaderID;
	GLuint fragmentShaderID;
	GLuint geometryShaderID;
	Shader();
	void installShaders(const char* VertexShaderName, const char* FragmentShaderName);
	void installShaders(const char * VertexShaderName, const char * FragmentShaderName, const char * GeometryShaderName);
	bool checkShaderStatus(GLint shaderID);
	bool checkProgramStatus();
	std::string readShaderCode(const char * filename);
	~Shader();
};


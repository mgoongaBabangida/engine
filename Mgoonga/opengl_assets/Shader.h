#pragma once
#include "stdafx.h"
#include <glew-2.1.0\include\GL\glew.h>
#include <base/base.h>
#include <glm/glm/glm.hpp>

//------------------------------------------------------
class Shader
{
public:
	Shader() = default;
	~Shader();

	GLuint ID() { return id;}

	void					GetUniformInfoFromShader();
	void					GetUniformDataFromShader();
	const std::vector<Uniform>& GetUniforms() const { return uniforms; }
	
	void					installShaders(const char* VertexShaderName, const char* FragmentShaderName);
	void					installShaders(const char* VertexShaderName,
															 const char* FragmentShaderName,
															 const char* GeometryShaderName);//@todo make one function
	void					installShaders(const char* VertexShaderName,
															 const char* FragmentShaderName,
															 const char* Tessalation1ShaderName,
															 const char* Tessalation2ShaderName);//@todo make one function
	std::string		readShaderCode(const char* filename);

protected:
	bool					checkShaderStatus(GLint shaderID);
	bool					checkProgramStatus();

	GLuint id;

	GLuint vertexShaderID;
	GLuint fragmentShaderID;
	GLuint geometryShaderID;
	GLuint tessellation1ShaderID;
	GLuint tessellation2ShaderID;

	std::vector<Uniform> uniforms;
};


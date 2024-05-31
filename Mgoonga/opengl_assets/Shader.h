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
	const std::vector<Uniform>& GetUniforms() const { return m_uniforms; }
	
	bool SetUniformData(const std::string& _name, const UniformData& _data);

	void					installShaders(const char* ComputeShaderName);
	void					installShaders(const char* VertexShaderName,
															 const char* FragmentShaderName,
															 bool _transformFeedback = false);
	void					installShaders(const char* VertexShaderName,
															 const char* FragmentShaderName,
															 const char* GeometryShaderName,
															 bool _transformFeedback = false);//@todo make one function
	void					installShaders(const char* VertexShaderName,
															 const char* FragmentShaderName,
															 const char* Tessalation1ShaderName,
															 const char* Tessalation2ShaderName);//@todo make one function
	std::string		readShaderCode(const char* filename);

protected:
	void _SetUniform(const Uniform& _data);

	bool					checkShaderStatus(GLint shaderID);
	bool					checkProgramStatus();

	GLuint id = UINT32_MAX;

	GLuint vertexShaderID;
	GLuint fragmentShaderID;
	GLuint geometryShaderID;
	GLuint tessellation1ShaderID;
	GLuint tessellation2ShaderID;
	GLuint computeShaderID;

	std::vector<Uniform> m_uniforms;
};


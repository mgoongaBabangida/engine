#pragma once

#include "Shader.h"
#include <math/Camera.h>
#include <base/base.h>
#include "MyMesh.h"

//-------------------------------------------------------------------
class eLinesRender
{
public:
	eLinesRender(const std::string& vertexShaderPath,
							 const std::string& fragmentShaderPath);
  ~eLinesRender();

	void Render(const Camera& camera, std::vector<const LineMesh*>& _meshes);

	Shader& GetShader() { return linesShader; }

protected:
	Shader	linesShader;
	
	GLuint	MVPLoc;

  GLuint	 VAO;
  GLuint	 VBO;
  GLuint	 EBO;
};
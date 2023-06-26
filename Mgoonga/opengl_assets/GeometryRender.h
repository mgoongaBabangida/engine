#pragma once
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include "Shader.h"

class SimpleGeometryMesh;

//----------------------------------------------------
class eGeometryRender
{
public:
	eGeometryRender(const std::string& vS, const std::string& fS, const std::string&	gS);
  ~eGeometryRender();

	void Render(glm::mat4 mvp, SimpleGeometryMesh& _mesh);

	Shader& GetShader() { return hex_shader; }
private:
	Shader hex_shader;
};

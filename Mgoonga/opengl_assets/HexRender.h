#pragma once
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include "Shader.h"

class SimpleGeometryMesh;

//----------------------------------------------------
class eHexRender
{
public:
	eHexRender(const std::string& vS, const std::string& fS, const std::string&	gS);
  ~eHexRender();

	void Render(glm::mat4 mvp, SimpleGeometryMesh& _mesh);

	Shader& GetShader() { return hex_shader; }
private:
	Shader hex_shader;

	GLuint MVPLoc;
	GLuint radiusLoc;
};

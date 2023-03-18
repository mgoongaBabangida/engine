#include "stdafx.h"
#include "HexRender.h"
#include "MyMesh.h"

//-------------------------------------------------------------------
eHexRender::eHexRender(const std::string&	vS,
					   const std::string&	fS,
					   const std::string&	gS)
{
	hex_shader.installShaders(vS.c_str(), fS.c_str(), gS.c_str());

	MVPLoc = glGetUniformLocation(hex_shader.ID(), "MVP");
	radiusLoc = glGetUniformLocation(hex_shader.ID(), "radius");
}

//---------------------------------------------------------------------------------------
eHexRender::~eHexRender()
{
}

//------------------------------------------------------------------------------------
void eHexRender::Render(glm::mat4 mvp, SimpleGeometryMesh& _mesh)
{
	glUseProgram(hex_shader.ID());
	glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &mvp[0][0]);
	glUniform1f(radiusLoc, 0.57f * _mesh.GetRadius()); //@todo what is 0.57?
	_mesh.Draw();
}

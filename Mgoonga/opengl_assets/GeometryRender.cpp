#include "stdafx.h"
#include "GeometryRender.h"
#include "MyMesh.h"

//-------------------------------------------------------------------
eGeometryRender::eGeometryRender(const std::string&	vS,
					   const std::string&	fS,
					   const std::string&	gS)
{
	hex_shader.installShaders(vS.c_str(), fS.c_str(), gS.c_str());
}

//---------------------------------------------------------------------------------------
eGeometryRender::~eGeometryRender()
{
}

//------------------------------------------------------------------------------------
void eGeometryRender::Render(glm::mat4 mvp, SimpleGeometryMesh& _mesh)
{
	glUseProgram(hex_shader.ID());
	hex_shader.SetUniformData("MVP", mvp);
	hex_shader.SetUniformData("radius", _mesh.GetRadius());
	hex_shader.SetUniformData("type", (int)_mesh.GetGeometryType());
	hex_shader.SetUniformData("color", _mesh.GetColor());
	_mesh.Draw();
}

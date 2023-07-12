#include "stdafx.h"
#include "BezierRender.h"

#include <math/Camera.h>
#include <opengl_assets/MyMesh.h>

//-------------------------------------
eBezierRender::eBezierRender(const std::string& vS, const std::string& fS, const std::string& tS1, const std::string& tS2)
{
  m_bezier_shader.installShaders(vS.c_str(), fS.c_str(), tS1.c_str(), tS2.c_str());
  glUseProgram(m_bezier_shader.ID());

  m_mvp = glGetUniformLocation(m_bezier_shader.ID(), "MVP");
  m_numSegments = glGetUniformLocation(m_bezier_shader.ID(), "NumSegments");
  m_numStrips = glGetUniformLocation(m_bezier_shader.ID(), "NumStrips");
}

//-------------------------------------
void eBezierRender::Render(const Camera& _camera, std::vector<const BezierCurveMesh*>& _meshes)
{
  glUseProgram(m_bezier_shader.ID());

  glUniform1i(m_numSegments, 30);
  glUniform1i(m_numStrips, 1);

  for (auto& cmesh : _meshes)
  {
    BezierCurveMesh* mesh = const_cast<BezierCurveMesh*>(cmesh);
    if (mesh->Is2D())
    {
      glUniformMatrix4fv(m_mvp, 1, GL_FALSE, &UNIT_MATRIX[0][0]);
    }
    else
    {
      glm::mat4 mvp = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix() * UNIT_MATRIX;
      glUniformMatrix4fv(m_mvp, 1, GL_FALSE, &mvp[0][0]);
    }
    mesh->Draw();
  }
}

//-------------------------------------
eBezierRender::~eBezierRender()
{
}
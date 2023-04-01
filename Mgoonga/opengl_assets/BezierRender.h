#pragma once

#include "Shader.h"

class Camera;
class BezierCurveMesh;

//------------------------------------
class eBezierRender
{
public:
  eBezierRender(const std::string& vS, const std::string& fS, const std::string& tS1, const std::string& tS2);
  ~eBezierRender();

  void Render(const Camera& _camera, std::vector<const BezierCurveMesh*>& _meshes);
  Shader& GetShader() { return m_bezier_shader; }
protected:
  Shader m_bezier_shader;

  GLuint m_mvp;
  GLuint m_numSegments;
  GLuint m_numStrips;
};

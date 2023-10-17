#include "stdafx.h"
#include "CSMRender.h"
#include <math/Transform.h>

//---------------------------------------------------------------------------------------------------
eCSMRender::eCSMRender(const std::string& vS, const std::string& fS, const std::string& gS)
  : bone_matrices(MAX_BONES)
{
  csmShader.installShaders(vS.c_str(), fS.c_str(), gS.c_str());
  glUseProgram(csmShader.ID());

  ModelLocationDir = glGetUniformLocation(csmShader.ID(), "model");
  BonesMatLocationDir = glGetUniformLocation(csmShader.ID(), "gBones");

  m_shadowCascadeLevels = { 2.0f, 4.0f, 8.0f, 13.0f };

  // configure UBO
// --------------------
  glGenBuffers(1, &matricesUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//---------------------------------------------------------------------------------------------------
eCSMRender::~eCSMRender()
{
}

//---------------------------------------------------------------------------------------------------
void eCSMRender::Render(const Camera& _camera, const Light& _light, const std::vector<shObject>& _objects)
{
  glUseProgram(csmShader.ID());

  float cameraFarPlane = _camera.getFarPlane();
  std::vector<glm::mat4> viewProjectionMatrices = _getLightSpaceMatrices(_camera, _light);
   
  glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
  for (size_t i = 0; i < viewProjectionMatrices.size(); ++i)
    glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &viewProjectionMatrices[i]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  //RENDER DEPTH
  for (auto& object : _objects)
  {
    glm::mat4 model = object->GetTransform()->getModelMatrix();
    glUniformMatrix4fv(ModelLocationDir, 1, GL_FALSE, &model[0][0]);

    if (object->GetRigger() != nullptr)
    {
      bone_matrices = object->GetRigger()->GetMatrices();
    }
    else
    {
      for (auto& m : bone_matrices)
        m = UNIT_MATRIX;
    }
    glUniformMatrix4fv(BonesMatLocationDir, MAX_BONES, GL_FALSE, &bone_matrices[0][0][0]);
    object->GetModel()->Draw();
  }
}

//---------------------------------------------------------------------------------------------------
std::vector<float> eCSMRender::GetCascadeFlaneDistances() const
{
  return m_shadowCascadeLevels;
}

//---------------------------------------------------------------------------------------------------
std::vector<glm::mat4> eCSMRender::_getLightSpaceMatrices(const Camera& _camera, const Light& _light)
{
  std::vector<glm::mat4> ret;
  for (size_t i = 0; i < m_shadowCascadeLevels.size() + 1; ++i)
  {
    if (i == 0)
    {
      ret.push_back(_getLightSpaceMatrix(_camera, _light, _camera.getNearPlane(), m_shadowCascadeLevels[i]));
    }
    else if (i < m_shadowCascadeLevels.size())
    {
      ret.push_back(_getLightSpaceMatrix(_camera, _light, m_shadowCascadeLevels[i - 1], m_shadowCascadeLevels[i]));
    }
    else
    {
      ret.push_back(_getLightSpaceMatrix(_camera, _light, m_shadowCascadeLevels[i - 1], _camera.getFarPlane()));
    }
  }
  return ret;
}

//---------------------------------------------------------------------------------------------------
glm::mat4 eCSMRender::_getLightSpaceMatrix(const Camera& _camera, const Light& _light, float _nearPlane,float _farPlane)
{
  Camera local_camera(_camera);
  local_camera.setNearPlane(_nearPlane);
  local_camera.setFarPlane(_farPlane);
  const auto corners = local_camera.getFrustumCornersWorldSpace();

  glm::vec3 center = glm::vec3(0, 0, 0);
  for (const auto& v : corners)
    center += glm::vec3(v);

  center /= corners.size();

  const auto lightView = glm::lookAt(center - glm::normalize(glm::vec3(_light.light_direction)), center, glm::vec3(0.0f, 1.0f, 0.0f));

  float minX = std::numeric_limits<float>::max();
  float maxX = std::numeric_limits<float>::lowest();
  float minY = std::numeric_limits<float>::max();
  float maxY = std::numeric_limits<float>::lowest();
  float minZ = std::numeric_limits<float>::max();
  float maxZ = std::numeric_limits<float>::lowest();
  for (const auto& v : corners)
  {
    const auto trf = lightView * v;
    minX = std::min(minX, trf.x);
    maxX = std::max(maxX, trf.x);
    minY = std::min(minY, trf.y);
    maxY = std::max(maxY, trf.y);
    minZ = std::min(minZ, trf.z);
    maxZ = std::max(maxZ, trf.z);
  }
  // Tune this parameter according to the scene
  if (minZ < 0)
  {
    minZ *= zMult;
  }
  else
  {
    minZ /= zMult;
  }
  if (maxZ < 0)
  {
    maxZ /= zMult;
  }
  else
  {
    maxZ *= zMult;
  }
  const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
  return lightProjection * lightView;
}

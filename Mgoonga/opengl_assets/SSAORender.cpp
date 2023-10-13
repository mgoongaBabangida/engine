#include "stdafx.h"
#include"SSAORender.h"
#include <math/Random.h>

//----------------------------------------------------
float ourLerp(float a, float b, float f)
{
  return a + f * (b - a);
}

//----------------------------------------------------
eSSAORender::eSSAORender(const std::string& _vSG,
                         const std::string& _fSG,
                         const std::string& _vS,
                         const std::string& _fS,
                         const std::string& _fSblur)
  : matrices(MAX_BONES)
{
  m_geometry_shader.installShaders(_vSG.c_str(), _fSG.c_str());
  m_main_shader.installShaders(_vS.c_str(), _fS.c_str());
  m_blur_shader.installShaders(_vS.c_str(), _fSblur.c_str());

  m_geometry_shader.GetUniformInfoFromShader();
  m_main_shader.GetUniformInfoFromShader();
  m_blur_shader.GetUniformInfoFromShader();

  screenMesh.reset(new eScreenMesh({}, {}));
  screenMesh->SetViewPortToDefault();

  // generate sample kernel
  for (unsigned int i = 0; i < 64; ++i)
  {
    glm::vec3 sample(math::Random::RandomFloat(0.0f, 1.0f) * 2.0 - 1.0,
                     math::Random::RandomFloat(0.0f, 1.0f) * 2.0 - 1.0,
                     math::Random::RandomFloat(0.0f, 1.0f));
    sample = glm::normalize(sample);
    sample *= math::Random::RandomFloat(0.0f, 1.0f);
    float scale = float(i) / 64.0f;

    // scale samples s.t. they're more aligned to center of kernel
    scale = ourLerp(0.1f, 1.0f, scale * scale);
    sample *= scale;
    m_ssao_kernel.push_back(glm::vec4(sample, 1.0f));
  }

  // generate noise texture
  std::vector<glm::vec3> ssaoNoise;
  for (unsigned int i = 0; i < 16; i++)
  {
    glm::vec3 noise(math::Random::RandomFloat(0.0f, 1.0f) * 2.0 - 1.0,
                    math::Random::RandomFloat(0.0f, 1.0f) * 2.0 - 1.0,
                    0.0f); // rotate around z-axis (in tangent space)
    ssaoNoise.push_back(noise);
  }

  glGenTextures(1, &noiseTexture);
  glBindTexture(GL_TEXTURE_2D, noiseTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

eSSAORender::~eSSAORender()
{
}

void eSSAORender::RenderGeometry(const Camera& camera, const Light& light, const std::vector<shObject>& objects)
{
  glUseProgram(m_geometry_shader.ID());

  m_geometry_shader.SetUniformData("projection", camera.getProjectionMatrix());
  m_geometry_shader.SetUniformData("view", camera.getWorldToViewMatrix());

  GLuint BonesMatLocation = glGetUniformLocation(m_geometry_shader.ID(), "gBones");
  for (auto& object : objects)
  {
    m_geometry_shader.SetUniformData("model", object->GetTransform()->getModelMatrix());
    if (object->GetRigger() != nullptr)
      matrices = object->GetRigger()->GetMatrices();
    else
    {
      for (auto& m : matrices)
        m = UNIT_MATRIX;
    }
    glUniformMatrix4fv(BonesMatLocation, MAX_BONES, GL_FALSE, &matrices[0][0][0]);
    object->GetModel()->Draw();
  }
}

//--------------------------------------------------
void eSSAORender::RenderSSAO(const Camera& camera)
{
  glUseProgram(m_main_shader.ID());

  // Send kernel + rotation 
  for (unsigned int i = 0; i < 64; ++i)
  {
    GLuint loc = glGetUniformLocation(m_main_shader.ID(), std::string("samples[" + std::to_string(i) + "]").c_str());
    glUniform4f(loc, m_ssao_kernel[i].x, m_ssao_kernel[i].y, m_ssao_kernel[i].z, 1.0f);
    //m_main_shader.SetUniformData("samples[" + std::to_string(i) + "]", m_ssao_kernel[i]); //@todo make it work
  }
  m_main_shader.SetUniformData("projection", camera.getProjectionMatrix());

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, noiseTexture);

  screenMesh->DrawUnTextured();
}

//--------------------------------------------------------
void eSSAORender::RenderSSAOBlur(const Camera& _camera)
{
  glUseProgram(m_blur_shader.ID());
  screenMesh->DrawUnTextured();
}

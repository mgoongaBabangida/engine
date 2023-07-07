#include "stdafx.h"
#include "IBLRender.h"
#include "CubeMesh.h"

eIBLRender::eIBLRender(const std::string& _vS, const std::string& _fS, const std::string& _irrfS, const Texture* _hdr)
: m_hdr(_hdr)
, m_cube(new CubeMesh)
{
  m_cubemap_shader.installShaders(_vS.c_str(), _fS.c_str());
  m_cubemap_shader.GetUniformInfoFromShader();

  m_irr_shader.installShaders(_vS.c_str(), _irrfS.c_str());
  m_irr_shader.GetUniformInfoFromShader();
}

eIBLRender::~eIBLRender()
{
}

void eIBLRender::RenderCubemap(const Camera& camera, GLuint _cubemap_id)
{
  //glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  float aspect = 1.0f; // (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT (depth buffer or viewport)
  glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), aspect, camera.getNearPlane(), camera.getFarPlane()); //should be 90
  glm::mat4 captureViews[] =
  {
     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
  };

  // convert HDR equirectangular environment map to cubemap equivalent
  glUseProgram(m_cubemap_shader.ID());

  m_cubemap_shader.SetUniformData("projection", captureProjection);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_hdr->id);

  for (unsigned int i = 0; i < 6; ++i)
  {
    m_cubemap_shader.SetUniformData("view", captureViews[i]);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _cubemap_id, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_cube->Draw();
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void eIBLRender::RenderIBLMap(const Camera& camera, GLuint _irr_id)
{
  //glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  float aspect = 1.0f; // (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT (depth buffer or viewport)
  glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), aspect, camera.getNearPlane(), camera.getFarPlane()); //should be 90
  glm::mat4 captureViews[] =
  {
     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
     glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
  };

  glUseProgram(m_irr_shader.ID());
  
  m_cubemap_shader.SetUniformData("projection", captureProjection);

  for (unsigned int i = 0; i < 6; ++i)
  {
    m_cubemap_shader.SetUniformData("view", captureViews[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _irr_id, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_cube->Draw();
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

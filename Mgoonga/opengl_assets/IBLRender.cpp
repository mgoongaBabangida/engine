#include "stdafx.h"
#include "IBLRender.h"
#include "CubeMesh.h"

eIBLRender::eIBLRender(const std::string& _vS, const std::string& _fS,
                       const std::string& _irrfS, const std::string& _prefilterfS,
                       const std::string& _brdfvS, const std::string& _brdffS, 
                       const Texture* _hdr)
: m_hdr(_hdr)
, m_cube(new CubeMesh)
{
  m_cubemap_shader.installShaders(_vS.c_str(), _fS.c_str());
  m_cubemap_shader.GetUniformInfoFromShader();

  m_irr_shader.installShaders(_vS.c_str(), _irrfS.c_str());
  m_irr_shader.GetUniformInfoFromShader();

  m_prefilter_shader.installShaders(_vS.c_str(), _prefilterfS.c_str());
  m_prefilter_shader.GetUniformInfoFromShader();

  m_brdf_shader.installShaders(_brdfvS.c_str(), _brdffS.c_str());
  m_brdf_shader.GetUniformInfoFromShader();

  screenMesh.reset(new eScreenMesh({}, {}));
  screenMesh->SetViewPortToDefault();

  m_captureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  m_captureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  m_captureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  m_captureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
  m_captureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  m_captureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

  // pbr: generate a 2D LUT from the BRDF equations used.
  // ----------------------------------------------------
  glGenTextures(1, &brdfLUTTexture);
  // pre-allocate enough memory for the LUT texture.
  glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
  // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

eIBLRender::~eIBLRender()
{
  glDeleteTextures(1, &brdfLUTTexture);
}

void eIBLRender::RenderCubemap(const Camera& camera, GLuint _cubemap_id)
{
  //glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  float aspect = 1.0f; // (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT (depth buffer or viewport)
  glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), aspect, camera.getNearPlane(), camera.getFarPlane()); //should be 90

  // convert HDR equirectangular environment map to cubemap equivalent
  glUseProgram(m_cubemap_shader.ID());

  m_cubemap_shader.SetUniformData("projection", captureProjection);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_hdr->id);

  for (unsigned int i = 0; i < 6; ++i)
  {
    m_cubemap_shader.SetUniformData("view", m_captureViews[i]);
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

  glUseProgram(m_irr_shader.ID());
  
  m_cubemap_shader.SetUniformData("projection", captureProjection);

  for (unsigned int i = 0; i < 6; ++i)
  {
    m_cubemap_shader.SetUniformData("view", m_captureViews[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _irr_id, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_cube->Draw();
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void eIBLRender::RenderPrefilterMap(const Camera& camera, GLuint _prefilter_id, GLuint _rbo_id)
{
  glUseProgram(m_prefilter_shader.ID());
  //prefilterShader.setInt("environmentMap", 0);

  float aspect = 1.0f; // (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT (depth buffer or viewport)
  glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), aspect, camera.getNearPlane(), camera.getFarPlane());
  m_prefilter_shader.SetUniformData("projection", captureProjection);

  unsigned int maxMipLevels = 5;
  for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
  {
    // reisze framebuffer according to mip-level size.
    unsigned int mipWidth = 128 * std::pow(0.5, mip);
    unsigned int mipHeight = 128 * std::pow(0.5, mip);
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
    glViewport(0, 0, mipWidth, mipHeight);

    float roughness = (float)mip / (float)(maxMipLevels - 1);
    m_prefilter_shader.SetUniformData("roughness", roughness);
    for (unsigned int i = 0; i < 6; ++i)
    {
      m_cubemap_shader.SetUniformData("view", m_captureViews[i]);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _prefilter_id, mip);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      m_cube->Draw();
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void eIBLRender::RenderBrdf()
{
  glUseProgram(m_brdf_shader.ID());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  screenMesh->DrawUnTextured();
}

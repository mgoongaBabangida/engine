#pragma once

#include "Shader.h"
#include <math/Camera.h>
#include <math/Clock.h>

struct Texture;

//@todo make a separate class for particle system on gpu to be able to have a few of them at the same time
//@todo make possible different functions for different types of particle systems as on cpu
//@todo make a mesh class for this particle rendering sys
//@todo make Particle and ParticleSys classes GPU/CPU compatible
//@todoinvestigate why geometry shader does not output proper transform feedback(GL_SEPARATE_ATTRIBS (?prob))
// OGLDev version does not work probably because of geometry shader


//------------------------------------------------------------------------------------
class eParticleSystemRenderGPU_V2
{
public:

  eParticleSystemRenderGPU_V2(const std::string&, const std::string&, const std::string&);
  ~eParticleSystemRenderGPU_V2();

  Shader& GetShader() { return m_renderShader; }

  void	Render(const Camera& camera);
  void	AddParticleSystem(glm::vec3 _startPos, const Texture* _texture);

protected:
  float randFloat();
  void _InitBuffers();

  Shader		m_renderShader;

  math::eClock    m_clock;
  const Texture*  m_particleTexture;
  glm::vec3       m_start_pos;

  int nParticles;

  float angle;
  float time, deltaT;

  GLuint feedback[2];
  GLuint posBuf[2];
  GLuint velBuf[2];
  GLuint startTime[2];
  GLuint particleArray[2];
  GLuint drawBuf = 1;
  GLuint initVel;

  GLuint m_updateSubLoc;
  GLuint m_renderSubLoc;

  GLuint m_mvp;
  GLuint m_gmvp;
  GLuint m_HLoc;
  GLuint m_timeLoc;
  GLuint m_lifeTimeLoc;
  GLuint m_numRowsLoc;
  GLuint m_CameraPosLoc;
  GLuint m_BillboardSizeLoc;
};


//not working
//------------------------------------------------------------------------------------
class eParticleSystemRenderGPU
{
public:

  struct Particle
  {
    float Type = 1.0f;
    glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Vel = glm::vec3(0.0f, 0.0001f, 0.0f);
    float LifetimeMillis = 0.0f;
  };

  static const int MAX_PARTICLES = 1000;

  eParticleSystemRenderGPU(const std::string&, const std::string&, const std::string&, const std::string&, const std::string&);
  ~eParticleSystemRenderGPU();

  Shader& GetShader() { return m_renderShader; }

  void	Render(const Camera& camera);
  void	AddParticleSystem(glm::vec3 _startPos, Texture* _texture);

  void _UpdateParticles();
  void _RenderParticles(const Camera& _camera);

protected:
  Shader		m_updateShader;
  Shader		m_renderShader;

  math::eClock m_clock;
  Texture* m_particleTexture;
  Texture* m_randomTexture;

  bool m_isFirst = true;
  unsigned int m_currVB = 0;
  unsigned int m_currTFB = 1;
  GLuint m_particleBuffer[2];
  GLuint m_transformFeedback[2];

  GLuint m_deltaTimeMillisLocation;
  GLuint m_randomTextureLocation;
  GLuint m_timeLocation;
  GLuint m_launcherLifetimeLocation;
  GLuint m_shellLifetimeLocation;

  GLuint m_VPLocation;
  GLuint m_cameraPosLocation;
  GLuint m_colorMapLocation;
  GLuint m_billboardSizeLocation;
};

#include "stdafx.h"

#include "ParticleSystemRenderGPUTransformfeedback.h"
#include "Texture.h"
#include "GlPipelineState.h"

//------------------------------------------------------------------------------
eParticleSystemRenderGPU::eParticleSystemRenderGPU(const std::string& _vertexUpdateShader,
                                                   const std::string& _fragmentShader,
                                                   const std::string& _geometryUpdateShader,
                                                   const std::string& _vertexRenderShader,
                                                   const std::string& _geometryRenderShader)
{
  m_updateShader.installShaders(_vertexUpdateShader.c_str(), _fragmentShader.c_str(), _geometryUpdateShader.c_str(), true);
  
  m_deltaTimeMillisLocation = glGetUniformLocation(m_updateShader.ID(), "gDeltaTimeMillis"); 
  m_randomTextureLocation = glGetUniformLocation(m_updateShader.ID(), "gRandomTexture");
  m_timeLocation = glGetUniformLocation(m_updateShader.ID(),"gTime");
  
  m_launcherLifetimeLocation = glGetUniformLocation(m_updateShader.ID(), "gLauncherLifetime");
  m_shellLifetimeLocation = glGetUniformLocation(m_updateShader.ID(), "gShellLifetime");

  m_renderShader.installShaders(_vertexRenderShader.c_str(), _fragmentShader.c_str(), _geometryRenderShader.c_str(), false);

  m_VPLocation = glGetUniformLocation(m_renderShader.ID(), "gVP");
  m_cameraPosLocation = glGetUniformLocation(m_renderShader.ID(), "gCameraPos");
  m_colorMapLocation = glGetUniformLocation(m_renderShader.ID(), "gColorMap");
  m_billboardSizeLocation = glGetUniformLocation(m_renderShader.ID(), "gBillboardSize");

  m_randomTexture = new Texture();
  m_randomTexture->makeRandom1DTexture(1000);
}

//------------------------------------------------------------------------------
eParticleSystemRenderGPU::~eParticleSystemRenderGPU()
{
  m_randomTexture->freeTexture();
  delete m_randomTexture;

  if (m_transformFeedback[0] != 0) {
    glDeleteTransformFeedbacks(2, m_transformFeedback);
  }

  if (m_particleBuffer[0] != 0) {
    glDeleteBuffers(2, m_particleBuffer);
  }
}

//------------------------------------------------------------------------------
void eParticleSystemRenderGPU::Render(const Camera& _camera)
{
  if (!m_clock.isActive())
    return;

  _UpdateParticles();
  _RenderParticles(_camera);

  m_currVB = m_currTFB;
  m_currTFB = (m_currTFB + 1) & 0x1;
}

//------------------------------------------------------------------------------
void eParticleSystemRenderGPU::AddParticleSystem(glm::vec3 _startPos, Texture* _texture)
{
  Particle Particles[MAX_PARTICLES];

  Particles[0].Type = 0.0f;
  Particles[0].Pos = _startPos;
  Particles[0].Vel = glm::vec3(0.0f, 0.0001f, 0.0f);
  Particles[0].LifetimeMillis = 0.0f;
  m_particleTexture = _texture;

  glUseProgram(m_updateShader.ID());
  glUniform1f(m_launcherLifetimeLocation, 10.0f);
  glUniform1f(m_shellLifetimeLocation, 100000.0f);

  //should be inside particle system
  glGenTransformFeedbacks(2, m_transformFeedback);
  glGenBuffers(2, m_particleBuffer);

  for (unsigned int i = 0; i < 2; i++) {
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
    glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
  }
  m_clock.start();
}

//------------------------------------------------------------------------------
void eParticleSystemRenderGPU::_UpdateParticles()
{
  glUseProgram(m_updateShader.ID());
  glUniform1f(m_timeLocation, static_cast<float>(m_clock.timeElapsedMsc()));
  glUniform1f(m_deltaTimeMillisLocation, static_cast<float>(m_clock.newFrame()));

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, m_randomTexture->m_id);

  eGlPipelineState::GetInstance().EnableRasterizerDiscard();

  glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currVB]);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]);

  glBeginTransformFeedback(GL_POINTS);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);                          // type
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);         // position
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)16);        // velocity
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)28);          // lifetime

  if (m_isFirst) {
    glDrawArrays(GL_POINTS, 0, 1);

    m_isFirst = false;
  }
  else {
    glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currVB]);
  }

  glEndTransformFeedback();

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);
}

//------------------------------------------------------------------------------
void eParticleSystemRenderGPU::_RenderParticles(const Camera& _camera)
{
  glUseProgram(m_renderShader.ID());

  glUniform1f(m_billboardSizeLocation, 0.01f);
  glUniform3f(m_cameraPosLocation, _camera.getPosition()[0], _camera.getPosition()[1], _camera.getPosition()[2]);
  glm::mat4 viewProjection = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix();
  glUniformMatrix4fv(m_VPLocation, 1, GL_FALSE, &viewProjection[0][0]);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_particleTexture->m_id);

  glDisable(GL_RASTERIZER_DISCARD);

  glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);

  glEnableVertexAttribArray(0);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);  // position

  glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);

  glDisableVertexAttribArray(0);
}



//---------------------------------------------------------------------------------------------
eParticleSystemRenderGPU_V2::eParticleSystemRenderGPU_V2(const std::string& _vertexShader, const std::string& _fragmentShader, const std::string& _geometryShader)
{
  m_renderShader.installShaders(_vertexShader.c_str(), _fragmentShader.c_str(), /*_geometryShader.c_str(),*/ true);

  m_renderSubLoc = glGetSubroutineIndex(m_renderShader.ID(), GL_VERTEX_SHADER, "render");
  m_updateSubLoc = glGetSubroutineIndex(m_renderShader.ID(), GL_VERTEX_SHADER, "update");

  m_mvp = glGetUniformLocation(m_renderShader.ID(), "MVP");
  m_gmvp = glGetUniformLocation(m_renderShader.ID(), "gMVP");
  m_HLoc = glGetUniformLocation(m_renderShader.ID(), "H");
  m_timeLoc = glGetUniformLocation(m_renderShader.ID(), "Time");
  m_lifeTimeLoc = glGetUniformLocation(m_renderShader.ID(), "ParticleLifetime");
  m_numRowsLoc = glGetUniformLocation(m_renderShader.ID(), "NumRowsInTexture");
  m_CameraPosLoc = glGetUniformLocation(m_renderShader.ID(), "gCameraPos");
  m_BillboardSizeLoc = glGetUniformLocation(m_renderShader.ID(), "gBillboardSize");
}

//--------------------------------------------------------------------------------------------
eParticleSystemRenderGPU_V2::~eParticleSystemRenderGPU_V2()
{
}

//--------------------------------------------------------------
void eParticleSystemRenderGPU_V2::Render(const Camera& _camera)
{
  if (!m_clock.isActive())
    return;

  glUseProgram(m_renderShader.ID());
  glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1 ,&m_updateSubLoc);

  // uniforms H Time
  glUniform1f(m_timeLoc, static_cast<float>(m_clock.timeElapsedMsc())/ 1000.0f);
  glUniform1f(m_HLoc, static_cast<float>(m_clock.newFrame())/ 1000.0f);

  // geometry
  glUniform1f(m_BillboardSizeLoc, 0.01f);
  glUniform3f(m_CameraPosLoc, _camera.getPosition()[0], _camera.getPosition()[1], _camera.getPosition()[2]);
  glm::mat4 viewProjection = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix();
  //set matrix
  glm::mat4 mvp = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix() * UNIT_MATRIX;
  glUniformMatrix4fv(m_mvp, 1, GL_FALSE, &mvp[0][0]);
  glUniformMatrix4fv(m_gmvp, 1, GL_FALSE, &mvp[0][0]);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_particleTexture->m_id);

  eGlPipelineState::GetInstance().EnableRasterizerDiscard();
  
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
  
  glBeginTransformFeedback(GL_POINTS);
    glBindVertexArray(particleArray[1-drawBuf]);
    glDrawArrays(GL_POINTS, 0, nParticles);
  glEndTransformFeedback();

  eGlPipelineState::GetInstance().DisableRasterizerDiscard();
  
  // Render pass
  glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_renderSubLoc);

  glBindVertexArray(particleArray[drawBuf]);
  glDrawTransformFeedback(GL_POINTS, feedback[drawBuf]);

  // Swap buffers
  drawBuf = 1 - drawBuf;
}

//---------------------------------------------------------------------------------------
void eParticleSystemRenderGPU_V2::AddParticleSystem(glm::vec3 _startPos, const Texture* _texture)
{
  m_start_pos = _startPos;
  glUseProgram(m_renderShader.ID());
  glUniform1f(m_lifeTimeLoc, 3.5f);
  m_renderShader.SetUniformData("Accel", glm::vec4(0.0f, -0.4f, 0.0f, 1.0f));
  m_renderShader.SetUniformData("StartPos", glm::vec4(m_start_pos, 1.0f));
  glUniform1i(m_numRowsLoc, 4); //@todo change for variable

  m_particleTexture = _texture;

  glPointSize(10.0f);
  _InitBuffers();

  m_clock.start();
}

//----------------------------------------------------------
void eParticleSystemRenderGPU_V2::_InitBuffers()
{
  nParticles = 4000;

  // Generate the buffers
  glGenBuffers(2, posBuf);    // position buffers
  glGenBuffers(2, velBuf);    // velocity buffers
  glGenBuffers(2, startTime); // Start time buffers
  glGenBuffers(1, &initVel);  // Initial velocity buffer (never changes, only need one)

  // Allocate space for all buffers
  int size = nParticles * 3 * sizeof(GLfloat);
  glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
  glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
  glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
  glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
  glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
  glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
  glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
  glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
  glBindBuffer(GL_ARRAY_BUFFER, initVel);
  glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, startTime[0]);
  glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_DYNAMIC_COPY);
  glBindBuffer(GL_ARRAY_BUFFER, startTime[1]);
  glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_DYNAMIC_COPY);

  // Fill the first position buffer with zeroes
  GLfloat* data = new GLfloat[nParticles * 3];
  for (int i = 0; i < nParticles * 3; i = i + 3)
  {
    data[i] = m_start_pos[0];
    data[i+1] = m_start_pos[1];
    data[i+2] = m_start_pos[2];
  }
  glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

  // Fill the first velocity buffer with random velocities
  glm::vec3 v(0.0f);
  float velocity, theta, phi;
  for (int i = 0; i < nParticles; i++) {

    theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, randFloat());
    phi = glm::mix(0.0f, glm::two_pi<float>(), randFloat());

    v.x = sinf(theta) * cosf(phi);
    v.y = cosf(theta);
    v.z = sinf(theta) * sinf(phi);

    velocity = glm::mix(1.25f, 1.5f, randFloat());
    v = glm::normalize(v) * velocity;

    data[3 * i] = v.x;
    data[3 * i + 1] = v.y;
    data[3 * i + 2] = v.z;
  }

  glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
  glBindBuffer(GL_ARRAY_BUFFER, initVel);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

  // Fill the first start time buffer
  delete[] data;
  data = new GLfloat[nParticles];
  float time = 0.0f;
  float rate = 0.01f;
  for (int i = 0; i < nParticles; i++) {
    data[i] = time;
    time += rate;
  }
  glBindBuffer(GL_ARRAY_BUFFER, startTime[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  delete[] data;

  // Create vertex arrays for each set of buffers
  glGenVertexArrays(2, particleArray);

  // Set up particle array 0
  glBindVertexArray(particleArray[0]);
  glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, startTime[0]);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, initVel);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(3);

  // Set up particle array 1
  glBindVertexArray(particleArray[1]);
  glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, startTime[1]);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, initVel);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(3);

  glBindVertexArray(0);

  // Setup the feedback objects
  glGenTransformFeedbacks(2, feedback);

  // Transform feedback 0
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, startTime[0]);

  // Transform feedback 1
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, startTime[1]);

  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

  GLint value;
  glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, &value);
  printf("MAX_TRANSFORM_FEEDBACK_BUFFERS = %d\n", value);
}

//----------------------------------------------------------
float eParticleSystemRenderGPU_V2::randFloat() {
  return ((float)rand() / RAND_MAX);
}
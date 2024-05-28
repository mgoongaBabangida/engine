#pragma once
#include "opengl_assets.h"

#include "Shader.h"
#include <base/base.h>
#include <math/Camera.h>
#include <math/Clock.h>
#include <opengl_assets/Texture.h>

//------------------------------------------
class eComputeShaderRender
{
public:
  eComputeShaderRender(const std::string& cS, const std::string& pScS,
    const std::string& pSvS, const std::string& pSfS,
    const std::string& ClothvS, const std::string& ClothfS,
    const std::string& ClothcS, const std::string& ClothNormcS,
    const std::string& EdgecS, const std::string& WorlycS,
    const std::string& vRes, const std::string& f3DDebug,
    const Texture* _textileTexture,
    const Texture* _computeShaderImage);
  ~eComputeShaderRender();

  void DispatchCompute(const Camera& _camera);
  void RenderComputeResult(const Camera& _camera);

  GLuint GetWorley3DID() { return m_worley3DID; }
  float& WorleyNoiseZDebug() { return m_WorleyZ; }
  int32_t& Noize3DOctaveDebug() { return m_WorleyOctave; }
  int32_t& GetOctaveSizeOne() { return mWorleyOctaveOneSize;}
  int32_t& GetOctaveSizeTwo() { return mWorleyOctaveTwoSize; }
  int32_t& GetOctaveSizeThree() { return mWorleyOctaveThreeSize; }
  void RedoNoise() { m_redo_noise = true; }
  int32_t& GetNoiseGamma() { return m_noise_gamma; }

  Shader& GetShader() { return mWorley3D; }
protected:
  void _DispatchSimpleCompute();

  void _DispatchEgdeDetection();

  void _InitPSCompute();
  void _DispatchPs(const Camera& _camera);
  void _RenderPS(const Camera& _camera);

  void _InitClothSimulCompute();
  void _DispatchClothSimul(const Camera& _camera);
  void _RenderClothSimul(const Camera& _camera);

  void _InitWorley3d();

  void DispatchWorley3D(const Camera& _camera);

  Shader mPSComputeShader;
  Shader mPSRenderResultShader;

  GLuint mTextileTextureId;
  Shader mRenderClothSimulShader, mComputeClothSimulShader, mComputeClothSimulNorm;

  Shader mEdgeFinding;
  Shader mWorley3D;;
  Shader mComputeShader;

  Shader mDebugShader;

  GLuint mImageId;
  GLuint mImageWidth;
  GLuint mImageHeight;

  GLuint m_worley3DID = Texture::GetDefaultTextureId();
  const GLuint mWorleyDim = 64;

  int32_t mWorleyOctaveOneSize = 6;
  alignas(16) std::vector<glm::vec4> mWorleyPoints1;
  GLuint mWorleyPointsBuffer1 = Texture::GetDefaultTextureId();

  int32_t mWorleyOctaveTwoSize = 10;
  alignas(16) std::vector<glm::vec4> mWorleyPoints2;
  GLuint mWorleyPointsBuffer2 = Texture::GetDefaultTextureId();

  int32_t mWorleyOctaveThreeSize = 12;
  alignas(16) std::vector<glm::vec4> mWorleyPoints3;
  GLuint mWorleyPointsBuffer3 = Texture::GetDefaultTextureId();

  float m_WorleyZ = 0.5f;
  int32_t m_WorleyOctave = 0;
  int32_t m_noise_gamma = 142;
  bool m_redo_noise = false;

  //**************************************//
  struct ParticleSystemVars
  {
    glm::ivec3 nParticles;
    GLuint totalParticles;

    float time, deltaT, speed, angle;
    GLuint particlesVao;
    GLuint bhVao, bhBuf;  // black hole VAO and buffer
    glm::vec4 bh1, bh2;
    glm::vec3 att1, att2;
    math::eClock clock;

    ParticleSystemVars() :
      nParticles(100, 100, 100),
      time(0.0f), deltaT(0.0f), speed(35.0f), angle(0.0f),
      bh1(5, 0, 0, 1), bh2(-5, 0, 0, 1)
    {
      totalParticles = nParticles.x * nParticles.y * nParticles.z;
      clock.start();
    }
    void Update()
    {
      float t = (float)clock.newFrame();
      if (time == 0.0f)
        deltaT = 0.0f;
      else
        deltaT = t - time;

      time = t;
      angle += speed * deltaT;
      if (angle > 360.0f)
        angle -= 360.0f;
    }
  };
  ParticleSystemVars psVars;

  //**************************************//
  struct ClothSimulVars
  {
    GLuint clothVao;
    GLuint numElements;

    glm::ivec2 nParticles;  // Number of particles in each dimension
    glm::vec2 clothSize;    // Size of cloth in x and y

    float time, deltaT, speed;
    GLuint readBuf;
    GLuint posBufs[2], velBufs[2];
    GLuint normBuf, elBuf, tcBuf;
    ClothSimulVars() : clothVao(0), numElements(0),
      nParticles(40, 40), clothSize(4.0f, 3.0f),
      time(0.0f), deltaT(0.0f), speed(200.0f), readBuf(0)
    {}
    void Update(float t)
    {
      if (time == 0.0f) {
        deltaT = 0.0f;
      }
      else {
        deltaT = t - time;
      }
      time = t;
    }
  };
  ClothSimulVars clothVars;

  //************************************************//

};
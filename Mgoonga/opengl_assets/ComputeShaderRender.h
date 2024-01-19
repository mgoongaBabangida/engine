#pragma once
#include "opengl_assets.h"

#include "Shader.h"
#include <base/base.h>
#include <math/Camera.h>
#include <math/Clock.h>

//------------------------------------------
class eComputeShaderRender
{
public:
  eComputeShaderRender(const std::string& cS, const std::string& pScS,
    const std::string& pSvS, const std::string& pSfS,
    const std::string& ClothvS, const std::string& ClothfS,
    const std::string& ClothcS, const std::string& ClothNormcS,
    const std::string& EdgecS,
    const Texture* _textileTexture,
    const Texture* _computeShaderImage);
  void DispatchCompute(const Camera& _camera);
  void RenderComputeResult(const Camera& _camera);

  Shader& GetShader() { return mPSComputeShader; }
protected:
  void _DispatchSimpleCompute();

  void _DispatchEgdeDetection();

  void _InitPSCompute();
  void _DispatchPs(const Camera& _camera);
  void _RenderPS(const Camera& _camera);

  void _InitClothSimulCompute();
  void _DispatchClothSimul(const Camera& _camera);
  void _RenderClothSimul(const Camera& _camera);

  Shader mPSComputeShader;
  Shader mPSRenderResultShader;

  GLuint mTextileTextureId;
  Shader mRenderClothSimulShader, mComputeClothSimulShader, mComputeClothSimulNorm;

  Shader mEdgeFinding;

  Shader mComputeShader;

  GLuint mImageId;
  GLuint mImageWidth;
  GLuint mImageHeight;

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
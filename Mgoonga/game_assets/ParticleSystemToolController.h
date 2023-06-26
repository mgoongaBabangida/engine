#pragma once
#include <base/interfaces.h>

class IWindowImGui;
class eTextureManager;
class eSoundManager;
class eOpenGlRenderPipeline;

//--------------------------------------
class ParticleSystemToolController : public IScript
{
public:
  ParticleSystemToolController(IWindowImGui* _imgui,
															 eTextureManager* _texManager,
															 eSoundManager* _soundManager,
															 eOpenGlRenderPipeline& _pipeline);
	virtual void Update(float _tick) override {}

protected:
  std::shared_ptr<IParticleSystem> m_psystem;
};
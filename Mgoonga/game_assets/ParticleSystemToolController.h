#pragma once

#include <base/interfaces.h>

class IWindowImGui;
class eMainContextBase;
class eModelManager;
class eTextureManager;
class eSoundManager;
class eOpenGlRenderPipeline;

//--------------------------------------
class ParticleSystemToolController : public IScript
{
public:
  ParticleSystemToolController(eMainContextBase* _game,
															 IWindowImGui* _imgui,
															 eModelManager* _modelManager,
															 eTextureManager* _texManager,
															 eSoundManager* _soundManager,
															 eOpenGlRenderPipeline& _pipeline);
	virtual void Update(float _tick) override {}
	
	virtual~ParticleSystemToolController();
protected:
  std::shared_ptr<IParticleSystem> m_psystem;
};
#pragma once

#include <base/interfaces.h>

class IWindowImGui;
class eMainContextBase;
class eModelManager;
class eObject;
class eTextureManager;
class eOpenGlRenderPipeline;

//---------------------------------------------------------------------
class VolumetricCloudsTool : public IScript
{
public:
  VolumetricCloudsTool(eMainContextBase* _game,
                       eModelManager* _modelManager,
                       eTextureManager* _texManager,
                       eOpenGlRenderPipeline& _pipeline,
                       IWindowImGui* _imgui);
  virtual ~VolumetricCloudsTool();

  virtual void Update(float _tick) override;
  virtual void Initialize() override;

protected:
  eMainContextBase* m_game = nullptr;
  eModelManager* m_modelManager = nullptr;
  eTextureManager* m_texture_manager = nullptr;
  std::reference_wrapper<eOpenGlRenderPipeline> m_pipeline;
  IWindowImGui* m_imgui = nullptr;
};
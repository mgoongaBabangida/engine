#ifndef MAIN_CONTEXT_H
#define MAIN_CONTEXT_H

#include <base/base.h>

#include <math/Camera.h>
#include <math/CameraRay.h>

#include <opengl_assets/OpenGlRenderPipeline.h>
#include <opengl_assets/GUI.h>
#include <opengl_assets/TerrainModel.h>

#include <game_assets/MainContextBase.h>

class IWindowImGui;

//-------------------------------------------------------------------------------
class eMgoongaGameContext : public eMainContextBase
{
public:
	eMgoongaGameContext(eInputController*,
        std::vector<IWindowImGui*> _externalGui,
				const std::string& modelsPath, 
				const std::string& assetsPath, 
				const std::string& shadersPath);

  virtual ~eMgoongaGameContext();
	
	virtual void			InitializeGL()									override;
	virtual void			PaintGL()										    override;
  virtual uint32_t  GetFinalImageId()               override;

	virtual bool			OnMouseMove(uint32_t x, uint32_t y)				      override;
	virtual bool			OnKeyPress(uint32_t asci)						            override;
	virtual bool			OnMousePress(uint32_t x, uint32_t y, bool left) override;
	virtual bool			OnMouseRelease()								                override;

  void OnFocusedChanged();
  void _InitializeHexes();

protected:
	virtual void			InitializePipline()								override;
	virtual void			InitializeBuffers()								override;
	virtual void			InitializeModels()								override;
	virtual void			InitializeRenders()								override;
	virtual void			InitializeSounds()								override;
  virtual void      InitializeExternalGui()           override;

protected:
  eOpenGlRenderPipeline							     pipeline;

  shObject							                 m_focused;
  std::vector<shObject>				           m_objects;
  std::vector<shObject>                  m_pbr_objs;
  std::shared_ptr<std::vector<shObject>> m_framed;
  std::vector<std::shared_ptr<GUI>>			 guis;
  shObject														   hex_model;

  //should be inside script
  std::optional<dbb::line>               m_grab_camera_line = std::nullopt;
  glm::vec3                              m_intersaction;
  glm::vec3                              m_grab_translation;
  glm::vec3                              m_translation_vector = glm::vec3{ 0.f,0.f,0.0f };
  
  //debuging
  shObject							                 lightObject;
};

#endif


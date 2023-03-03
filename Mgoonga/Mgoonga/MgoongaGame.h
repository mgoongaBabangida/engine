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

protected:
	virtual void			InitializePipline()								override;
	virtual void			InitializeBuffers()								override;
	virtual void			InitializeModels()								override;
	virtual void			InitializeRenders()								override;
	virtual void			InitializeSounds()								override;
  virtual void      InitializeExternalGui()           override;

protected:
  Light								                   m_light;
	Camera								                 m_camera;

  eOpenGlRenderPipeline							     pipeline;

  shObject							                 m_focused;
  std::vector<shObject>				           m_objects;
  std::vector<shObject>                  m_pbr_objs;
  std::shared_ptr<std::vector<shObject>> m_framed;
  std::vector<std::shared_ptr<GUI>>			 guis;

  //should be inside script
  std::optional<dbb::line>               m_grab_camera_line = std::nullopt;
  glm::vec3                              m_intersaction;
  glm::vec3                              m_grab_translation;
  glm::vec3                              m_translation_vector = glm::vec3{ 0.f,0.f,0.0f };

  shObject							                 lightObject; //debuging

  eThreeFloatCallback            transfer_data_position;
  eThreeFloatCallback            transfer_data_rotation;
  eThreeFloatCallback            transfer_data_scale;
  size_t                         transfer_num_vertices = 0;
  size_t                         transfer_num_meshes = 0;
  eVectorStringsCallback         transfer_meshes;
  std::vector<const Texture*>    transfer_textures;
  size_t                         transfer_num_animations = 0;
  eVectorStringsCallback         transfer_animations;
  size_t                         cur_animation = 0;
  std::vector<const IAnimation*> current_animations;
  std::function<void()>          play_callback;
  std::function<void()>          stop_callback;
  std::function<void()>          emit_partilces_callback;
};

#endif


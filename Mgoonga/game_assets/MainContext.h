#ifndef MAIN_CONTEXT_H
#define MAIN_CONTEXT_H

#include "game_assets.h"

#include <base/base.h>

#include <math/Camera.h>
#include <math/CameraRay.h>

#include <opengl_assets/Pipeline.h>
#include <opengl_assets/GUI.h>
#include <opengl_assets/TerrainModel.h>

#include <sdl_assets/MainContextBase.h>

class IWindowImGui;

//-------------------------------------------------------------------------------
class DLL_GAME_ASSETS eMgoongaGameContext : public eMainContextBase
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
	Camera								                 m_camera;
	dbb::CameraRay				                 camRay;
  ePipeline							                 pipeline;

  shObject							                 m_focused;
  std::vector<shObject>				           m_objects;
  std::vector<shObject>                  m_pbr_objs;
  std::shared_ptr<std::vector<shObject>> m_framed;
  Light								                   m_light;
  std::vector<GUI>					             guis;
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


#ifndef MAIN_CONTEXT_H
#define MAIN_CONTEXT_H

#include <base/base.h>

#include <math/Camera.h>
#include <math/CameraRay.h>

#include <opengl_assets/GUI.h>
#include <opengl_assets/TerrainModel.h>

#include <game_assets/MainContextBase.h>
#include <game_assets/InputStrategy.h>

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

	virtual void			PaintGL()										    override;

	virtual bool			OnMouseMove(uint32_t x, uint32_t y)				      override;
	virtual bool			OnKeyPress(uint32_t asci)						            override;
	virtual bool			OnMousePress(uint32_t x, uint32_t y, bool left) override;
	virtual bool			OnMouseRelease()								                override;

  void OnFocusedChanged();
  void _InitializeHexes();
  void _InitializeBezier();
  void _InitMainTestSceane();

protected:
	virtual void			InitializePipline()								override;
	virtual void			InitializeBuffers()								override;
	virtual void			InitializeModels()								override;
	virtual void			InitializeRenders()								override;
	virtual void			InitializeSounds()								override;
  virtual void      InitializeExternalGui()           override;

protected:
  std::vector<shObject>				           m_objects;
  std::vector<shObject>                  m_pbr_objs;
  std::shared_ptr<std::vector<shObject>> m_framed;
  std::vector<std::shared_ptr<GUI>>			 guis;
  shObject														   hex_model;
  std::array<shObject, 5>								 bezier_model;
  std::unique_ptr<InputStrategy>         m_inputStrategy;
  //debuging
  shObject							                 m_lightObject;
};

#endif


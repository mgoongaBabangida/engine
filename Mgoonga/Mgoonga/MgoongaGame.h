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

	virtual bool			OnKeyPress(uint32_t asci)						          override;
	virtual bool			OnMousePress(int32_t x, int32_t y, bool left) override;

  void OnFocusedChanged();
  void _InitializeHexes();
  void _InitMainTestSceane();

protected:
	virtual void			InitializePipline()								override;
	virtual void			InitializeBuffers()								override;
	virtual void			InitializeModels()								override;
	virtual void			InitializeRenders()								override;
	virtual void			InitializeSounds()								override;
  virtual void      InitializeExternalGui()           override;

protected:

};

#endif


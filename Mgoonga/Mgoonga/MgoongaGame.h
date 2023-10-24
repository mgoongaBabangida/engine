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

  void _InitializeHexes();
  void _InitMainTestSceane();

protected:
	virtual void			InitializeModels()								override;
};

#endif


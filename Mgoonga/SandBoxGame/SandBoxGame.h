#ifndef SAND_BOX_GAME_H
#define SAND_BOX_GAME_H

#include <base/base.h>

#include <math/Camera.h>
#include <math/CameraRay.h>

#include <game_assets/MainContextBase.h>

class IWindowImGui;

//-------------------------------------------------------------------------------------
class eSandBoxGame : public eMainContextBase
{
public:
	eSandBoxGame(eInputController*,
    std::vector<IWindowImGui*> _externalGui,
				const std::string& modelsPath,
				const std::string& assetsPath,
				const std::string& shadersPath);
	virtual ~eSandBoxGame() = default;

protected:
	virtual void		InitializeModels()			override;
};

#endif //SAND_BOX_GAME_H

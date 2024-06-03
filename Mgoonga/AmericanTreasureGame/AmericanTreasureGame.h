#ifndef AMERICAN_TREASURE_H
#define AMERICAN_TREASURE_H


#include <base/base.h>

#include <math/Camera.h>
#include <math/CameraRay.h>

#include <opengl_assets/GUI.h>
#include <opengl_assets/TerrainModel.h>

#include <game_assets/MainContextBase.h>
#include <game_assets/InputStrategy.h>

class IWindowImGui;

//-------------------------------------------------------------------------------
class AmericanTreasureGame : public eMainContextBase
{
public:
	AmericanTreasureGame(eInputController*,
		std::vector<IWindowImGui*>& _externalGui,
		const std::string& modelsPath,
		const std::string& assetsPath,
		const std::string& shadersPath,
		int _width,
		int _height);

	virtual ~AmericanTreasureGame();

protected:
	virtual void			InitializeModels()								override;
};
#endif //AMERICAN_TREASURE_H


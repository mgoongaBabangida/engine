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
		std::vector<IWindowImGui*> _externalGui,
		const std::string& modelsPath,
		const std::string& assetsPath,
		const std::string& shadersPath);

	virtual ~AmericanTreasureGame();

	virtual void			PaintGL()										    override;

	virtual bool			OnMouseMove(int32_t x, int32_t y)				      override;
	virtual bool			OnKeyPress(uint32_t asci)						            override;
	virtual bool			OnMousePress(int32_t x, int32_t y, bool left) override;
	virtual bool			OnMouseRelease()								                override;

	void OnFocusedChanged();
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
#endif //AMERICAN_TREASURE_H


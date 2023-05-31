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

	virtual bool			OnMouseMove(int32_t x, int32_t y)							override;
	virtual bool			OnKeyPress(uint32_t asci)												override;
	virtual bool			OnMousePress(int32_t x, int32_t y, bool left) override;
	virtual bool			OnMouseRelease()																override;

protected:

	virtual void		InitializePipline()			override;
	virtual void		InitializeBuffers()			override;
	virtual void		InitializeModels()			override;
	virtual void		InitializeRenders()			override;
	virtual void		InitializeSounds()			override;
};

#endif //SAND_BOX_GAME_H

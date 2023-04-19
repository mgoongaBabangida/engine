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

	virtual void			PaintGL()																				override;

	virtual bool			OnMouseMove(uint32_t x, uint32_t y)							override;
	virtual bool			OnKeyPress(uint32_t asci)												override;
	virtual bool			OnMousePress(uint32_t x, uint32_t y, bool left) override;
	virtual bool			OnMouseRelease()																override;

protected:
	std::vector<shObject>								m_objects;
	std::vector<shObject>								m_framed;
	std::vector<std::shared_ptr<GUI>>		guis;
	//debuging
	shObject							              m_lightObject;

	virtual void		InitializePipline()			override;
	virtual void		InitializeBuffers()			override;
	virtual void		InitializeModels()			override;
	virtual void		InitializeRenders()			override;
	virtual void		InitializeSounds()			override;
	virtual void		InitializeExternalGui() override;
};

#endif //SAND_BOX_GAME_H

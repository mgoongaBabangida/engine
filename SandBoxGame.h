#ifndef SAND_BOX_GAME_H
#define SAND_BOX_GAME_H

#include "MainContextBase.h"
#include "Structures.h"
#include "Pipeline.h"
#include "Camera.h"
#include "CameraRay.h"

class IWindowImGui;

class eSandBoxGame : public eMainContextBase
{
public:
	eSandBoxGame(eInputController*,
				IWindowImGui*,
				const std::string& modelsPath,
				const std::string& assetsPath,
				const std::string& shadersPath);
	virtual ~eSandBoxGame() = default;

	virtual void			InitializeGL()									override;
	virtual void			PaintGL()										override;

	virtual bool			OnMouseMove(uint32_t x, uint32_t y)				override;
	virtual bool			OnKeyPress(uint32_t asci)						override;
	virtual bool			OnMousePress(uint32_t x, uint32_t y, bool left) override;
	virtual bool			OnMouseRelease()								override;
protected:
	Camera								m_camera;
	dbb::CameraRay						camRay;

	shObject							m_focused;
	std::vector<shObject>				m_Objects;
	std::vector<shObject>				m_framed;
	Light								m_light;

	virtual void						InitializePipline() override;
	virtual void						InitializeBuffers() override;
	virtual void						InitializeModels()	override;

	ePipeline							pipeline;
};

#endif //SAND_BOX_GAME_H

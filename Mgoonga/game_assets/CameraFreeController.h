#pragma once

#include "game_assets.h"

#include <base/interfaces.h>
#include <math/Camera.h>

//------------------------------------------------------------
class DLL_GAME_ASSETS CameraFreeController : public IScript
{
public:
 CameraFreeController(Camera& _camera, bool _strafe_shift = false);
 virtual ~CameraFreeController();

 virtual void		Update(float _tick) override;

 virtual bool		OnMouseMove(int32_t _x, int32_t _y, KeyModifiers _modifiers)	override;
 virtual bool		OnKeyJustPressed(uint32_t _asci, KeyModifiers _modifier)		override;
 virtual bool		OnKeyPress(const std::vector<bool>, KeyModifiers _modifier)		override;
 virtual bool		OnMouseWheel(int32_t x, int32_t y, KeyModifiers _modifiers)		override;

protected:
 std::reference_wrapper<Camera> m_camera;
 bool m_strafe_shift = false;
};
#pragma once

#include "game_assets.h"

#include <base/interfaces.h>
#include <math/Camera.h>

//------------------------------------------------------------
class DLL_GAME_ASSETS CameraFreeController : public IScript
{
public:
  explicit CameraFreeController(Camera&);
  virtual ~CameraFreeController();

  virtual void		Update(float _tick) override;

  virtual bool		OnMouseMove(int32_t _x, int32_t _y) override;
  virtual bool		OnKeyPress(uint32_t _asci)			override;
  virtual bool		OnMouseWheel(int32_t x, int32_t y) override;

protected:
  std::reference_wrapper<Camera> m_camera;
};
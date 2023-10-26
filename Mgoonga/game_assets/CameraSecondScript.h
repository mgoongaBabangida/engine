#pragma once
#include <base/interfaces.h>
#include <math/Camera.h>

//----------------------------------------------------------------
class CameraSecondScript : public IScript
{
public:
  CameraSecondScript(Camera*, IGame*);
  virtual void		Update(float _tick) override;

protected:
  Camera* m_camera = nullptr;
  IGame* m_game = nullptr;
};
#pragma once
#include "game_assets.h"

#include <base/interfaces.h>
#include <base/Object.h>

class eMainContextBase;
class LineMesh;

//-------------------------------------------
class DLL_GAME_ASSETS PhysicsEngineTestScript : public IScript
{
public:
  PhysicsEngineTestScript(eMainContextBase* _game);
  virtual ~PhysicsEngineTestScript();

  virtual bool  OnKeyPress(uint32_t _asci, KeyModifiers _modifier) override;
  virtual void	Update(float _tick) override;
  virtual void  Initialize() override;

protected:
  eMainContextBase* m_game = nullptr;
  shObject sphere1;
  shObject cube1;
  shObject sphere2;
  shObject cube2;
  std::vector<shObject> m_visual;
  LineMesh* m_normal_mesh = nullptr;
};
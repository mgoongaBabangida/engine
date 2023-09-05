#pragma once

#include "game_assets.h"

#include <base/interfaces.h>

class eMainContextBase;

//------------------------------------------
class DLL_GAME_ASSETS AnimationSocketScript : public IScript
{
public:
  explicit AnimationSocketScript(eMainContextBase* _game);
  virtual void Initialize() override;
  virtual void Update(float _tick) override;
protected:
  eMainContextBase* m_game = nullptr;
  unsigned int sockets_count = 0;
};
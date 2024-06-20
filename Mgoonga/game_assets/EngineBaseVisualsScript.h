#pragma once
#include "game_assets.h"

#include <base/interfaces.h>
#include <base/Object.h>

class eMainContextBase;
class LineMesh;

//-------------------------------------------
class EngineBaseVisualsScript : public IScript
{
public:
  EngineBaseVisualsScript(eMainContextBase* _game);
  virtual ~EngineBaseVisualsScript();

  virtual void	Update(float _tick) override;
  virtual void  Initialize() override;

  bool& GetVisibility() { return m_is_visible; }

protected:
  eMainContextBase* m_game = nullptr;
  shObject m_grid;
  shObject arrowX;
  shObject arrowY;
  shObject arrowZ;
  std::vector<std::shared_ptr<Text>> m_labels;
  bool m_is_visible = true;
};
#pragma once
#include "game_assets.h"
#include <base/interfaces.h>

class eModelManager;
class LineMesh;

//---------------------------------------
class DLL_GAME_ASSETS ShootScript : public IScript
{
public:
  ShootScript(IGame* _game, eModelManager* _modelManager);
  virtual ~ShootScript();
  virtual bool  OnKeyJustPressed(uint32_t _asci, KeyModifiers _modifier) override;
  virtual void	Update(float _tick) override;
  virtual void Initialize() override;

protected:
  IGame* m_game = nullptr;
  eModelManager* m_modelManager = nullptr;
  std::vector<std::shared_ptr<eObject>> m_bullets;
  std::shared_ptr<eObject> m_obj_to_add;
  LineMesh* m_normal_mesh = nullptr;
  bool m_deubg_normals = false;
  bool m_delete_all = false;
  unsigned int m_counter = 0;
};

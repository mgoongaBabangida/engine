#pragma once
#include "stdafx.h"
#include "game_assets.h"

class eObject;
class eModelManager;

class SceneSerializer
{
public:
  SceneSerializer(std::vector<std::shared_ptr<eObject>> _objects, eModelManager& _model_manager);
  SceneSerializer(const SceneSerializer&) = delete;

  void Serialize(std::string _filepath);
  std::vector<std::shared_ptr<eObject>> Deserialize(std::string _filepath);

private:
  std::vector<std::shared_ptr<eObject>> m_objects;
  std::reference_wrapper <eModelManager> m_model_manager;
};
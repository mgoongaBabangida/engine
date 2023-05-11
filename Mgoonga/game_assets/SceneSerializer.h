#pragma once
#include "stdafx.h"
#include "game_assets.h"

class eObject;
class eModelManager;
class AnimationManagerYAML;

class SceneSerializer
{
public:
  SceneSerializer(std::vector<std::shared_ptr<eObject>> _objects,
    eModelManager& _model_manager,
    AnimationManagerYAML& _animationManager);
  SceneSerializer(const SceneSerializer&) = delete;

  void Serialize(const std::string& _filepath);
  std::vector<std::shared_ptr<eObject>> Deserialize(const std::string& _filepath);

private:
  std::vector<std::shared_ptr<eObject>> m_objects;
  std::reference_wrapper <eModelManager> m_model_manager;
  std::reference_wrapper <AnimationManagerYAML> m_animation_manager;
};
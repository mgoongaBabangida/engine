#pragma once
#include "stdafx.h"

#include "game_assets.h"

class eObject;
class ModelManagerYAML;
class AnimationManagerYAML;

//-------------------------------------------------------------------------------------
class SceneSerializerYAML
{
public:
  SceneSerializerYAML(std::vector<std::shared_ptr<eObject>> _objects,
                      ModelManagerYAML& _model_manager,
                      AnimationManagerYAML& _animationManager);

  SceneSerializerYAML(const SceneSerializerYAML&) = delete;

  void Serialize(const std::string& _filepath);
  std::vector<std::shared_ptr<eObject>> Deserialize(const std::string& _filepath);

private:
  std::vector<std::shared_ptr<eObject>> m_objects;
  std::reference_wrapper <ModelManagerYAML> m_model_manager;
  std::reference_wrapper <AnimationManagerYAML> m_animation_manager;
};
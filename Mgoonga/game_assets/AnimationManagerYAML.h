#pragma once
#include "stdafx.h"

#include "game_assets.h"

#include <math/SceletalAnimation.h>

#include <optional>

class Rigger;

//--------------------------------------------------------------
class DLL_GAME_ASSETS AnimationManagerYAML
{
public:
  void AddAnimation(const SceletalAnimation&);
  std::optional<SceletalAnimation> GetAnimation(const std::string& _name) const;

  void Serialize(const std::string& _filepath);
  bool Deserialize(const std::string& _filepath);

  void SerializeRigger(const Rigger* _riger, const std::string& _filepath);
  IRigger* DeserializeRigger(const std::string& _filepath);

protected:
  std::vector<SceletalAnimation> m_animation;
};

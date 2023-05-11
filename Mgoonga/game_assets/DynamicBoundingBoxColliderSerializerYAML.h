#pragma once
#include "stdafx.h"
#include "game_assets.h"

class BoxColliderDynamic;

class DynamicBoundingBoxColliderSerializerYAML
{
public:
  void Serialize(BoxColliderDynamic* _box, const std::string& _filepath);
  BoxColliderDynamic* Deserialize(const std::string& _filepath);
};

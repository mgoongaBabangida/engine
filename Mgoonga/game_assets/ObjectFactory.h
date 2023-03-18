#pragma once
#include "stdafx.h"
#include "game_assets.h"

class eObject;
class IModel;

class DLL_GAME_ASSETS ObjectFactoryBase
{
public:
  std::unique_ptr<eObject> CreateObject(std::shared_ptr<IModel>, const std::string& _name = std::string(""));
};


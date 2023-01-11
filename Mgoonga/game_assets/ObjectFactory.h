#pragma once

#include "game_assets.h"

#include <memory>

class eObject;
class IModel;

class DLL_GAME_ASSETS ObjectFactoryBase
{
public:
  std::unique_ptr<eObject> CreateObject(std::shared_ptr<IModel>);
};


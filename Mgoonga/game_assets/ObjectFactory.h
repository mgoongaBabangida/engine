#pragma once
#include "stdafx.h"
#include "game_assets.h"
#include <base/Object.h>

class IModel;

class DLL_GAME_ASSETS ObjectFactoryBase
{
public:
  std::unique_ptr<eObject> CreateObject(std::shared_ptr<IModel>,
                                        eObject::RenderType _render_type,
                                        const std::string& _name = std::string("default"),
                                        bool _dynamic_collider = false);
};


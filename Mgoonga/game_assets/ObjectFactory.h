#pragma once
#include "stdafx.h"
#include "game_assets.h"
#include <base/Object.h>

class IModel;
class AnimationManagerYAML;

//--------------------------------------------------------------------------------------------
class DLL_GAME_ASSETS ObjectFactoryBase
{
public:
  explicit ObjectFactoryBase(AnimationManagerYAML* _animationManager = nullptr)
    : m_animationManager(_animationManager)
  {}

  std::unique_ptr<eObject> CreateObject(std::shared_ptr<IModel>,
                                        eObject::RenderType _render_type,
                                        const std::string& _name = std::string("default"));

  std::unique_ptr<eObject> CreateObject(std::shared_ptr<IModel>,
                                        eObject::RenderType _render_type,
                                        const std::string& _name,
                                        const std::string& _rigger_path,
                                        const std::string& _collider_path,
                                        bool _dynamic_collider = false);
protected:
  AnimationManagerYAML* m_animationManager = nullptr;
};

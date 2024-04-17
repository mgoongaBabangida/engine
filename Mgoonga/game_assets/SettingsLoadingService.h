#pragma once

#include "game_assets.h"

#include <base/interfaces.h>

#include <string>

class eOpenGlRenderPipeline;
class ModelManagerYAML;

//----------------------------------------------------------------------------------------------
class SettingsLoadingService
{
public:
  static void LoadPipelineSettings(const std::string& _path, IGame* _game, eOpenGlRenderPipeline& _pipeline);
  static void LoadModels(const std::string& _path, const std::string& _modelFolder, ModelManagerYAML* _modelManager, bool _multithreading);
  //@todo transfer loading textures from texture manager to here
};
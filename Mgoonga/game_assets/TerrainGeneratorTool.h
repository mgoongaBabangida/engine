#pragma once

#include <base/interfaces.h>
#include <glm/glm/gtc/noise.hpp>
#include <opengl_assets/Texture.h>

#include <vector>
#include <set>

class IWindowImGui;
class eMainContextBase;
class eModelManager;
class eObject;
class TerrainModel;
class eTextureManager;
class eOpenGlRenderPipeline;

struct TerrainType
{
  std::string name;
  float       threshold_start;
  float       threshold_finish;
  glm::vec3   color;
};

bool operator<(const TerrainType& _one, const TerrainType& _two);

class TerrainGeneratorTool : public IScript
{
public:
  TerrainGeneratorTool(eMainContextBase* _game,
    eModelManager*  _modelManager,
    eTextureManager* _texManager,
    eOpenGlRenderPipeline& _pipeline,
    IWindowImGui* _imgui);
  virtual ~TerrainGeneratorTool();

  virtual void Update(float _tick) override;
  virtual void Initialize() override;

protected:
  void _GenerateNoiseMap(GLuint _width, GLuint _height, float _scale, GLuint octaves,
                         float persistance, float lacunarity, glm::vec2 offfset, GLuint _seed);

  void _GenerateColorMap();

  std::vector<GLfloat>     m_noise_map;
  std::vector<glm::vec4>   m_color_map;
  Texture                  m_noise_texture;
  Texture                  m_color_texture;
  std::set<TerrainType>    m_terrain_types;
  std::shared_ptr<eObject> m_terrain;
  TerrainModel*            m_terrain_pointer;

  GLuint    m_width = 241;
  GLuint    m_height = 241;
  float     m_scale = 100.0f;
  GLuint    m_octaves = 4;
  float     m_persistance = 0.5f;
  float     m_lacunarity = 2.0f;
  glm::vec2 m_noise_offset = {0.0f, 0.0f};
  GLuint    m_seed = 1;
  float     m_height_scale = 7.0f;
  float     m_texture_scale[8];

  eMainContextBase* m_game = nullptr;
  eModelManager*    m_modelManager = nullptr;
  eTextureManager*  m_texture_manager = nullptr;
  std::reference_wrapper<eOpenGlRenderPipeline> m_pipeline;
  IWindowImGui* m_imgui = nullptr;
};

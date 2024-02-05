#pragma once

#include <base/interfaces.h>
#include <math/Bezier.h>
#include <glm/glm/gtc/noise.hpp>
#include <opengl_assets/Texture.h>
#include <opengl_assets/TerrainModel.h>

#include <vector>
#include <set>

class IWindowImGui;
class eMainContextBase;
class eModelManager;
class eObject;
class eTextureManager;
class eOpenGlRenderPipeline;

//---------------------------------------------------------------------
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

  void _UpdateCurrentMesh();
  void _AddCurrentMesh();

  void _GenerateFallOffMap();

  std::vector<GLfloat>     m_noise_map;
  std::vector<glm::vec4>   m_color_map;
  std::vector<GLfloat>     m_falloff_map;
  Texture                  m_noise_texture;
  Texture                  m_color_texture;
  std::set<TerrainType>    m_terrain_types;
  std::shared_ptr<eObject> m_terrain;
  TerrainModel*            m_terrain_pointer = nullptr;

  int         m_cur_pos_X = 0;
  int         m_cur_pos_Y = 0;
  GLuint      m_width = 257;
  GLuint      m_height = 257;
  float       m_scale = 100.0f;
  GLuint      m_octaves = 4;
  float       m_persistance = 0.5f;
  float       m_lacunarity = 2.0f;
  glm::vec2   m_noise_offset = {0.0f, 0.0f};
  GLuint      m_seed = 1;
  float       m_height_scale = 7.0f;
  float       m_texture_scale[8];
  float       m_max_height_coef = 1.0f;
  float       m_min_height_coef = 0.0f;
  dbb::Bezier m_interpolation_curve;
  bool        m_use_curve = false;
  bool        m_initialized = false;
  bool        m_auto_update = false;
  bool        m_apply_falloff = false;

  eMainContextBase* m_game = nullptr;
  eModelManager*    m_modelManager = nullptr;
  eTextureManager*  m_texture_manager = nullptr;
  std::reference_wrapper<eOpenGlRenderPipeline> m_pipeline;
  IWindowImGui* m_imgui = nullptr;
};

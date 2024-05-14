#pragma once

#include <base/interfaces.h>
#include <math/Bezier.h>
#include <glm/glm/gtc/noise.hpp>
#include <opengl_assets/Texture.h>
#include <opengl_assets/TerrainModel.h>

#include <vector>
#include <set>
#include <future>

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
  void _UpdateShaderUniforms();

  void _ApplyGaussianBlur();

  std::future<bool>        m_generat_noise_task;
  std::vector<GLfloat>     m_noise_map;
  std::vector<glm::vec4>   m_color_map;
  std::vector<GLfloat>     m_falloff_map;
  Texture                  m_noise_texture;
  Texture                  m_color_texture;
  std::set<TerrainType>    m_terrain_types;
  std::shared_ptr<eObject> m_terrain;
  TerrainModel*            m_terrain_pointer = nullptr;

  using eOctavesHeightsBuffer = std::vector<GLfloat>;
  std::vector<eOctavesHeightsBuffer> m_octaves_buffer;

  int         m_cur_pos_X = 0;
  int         m_cur_pos_Y = 0;
  GLuint      m_width = 1024;
  GLuint      m_height = 1024;
  int         m_scale = 500;
  GLuint      m_octaves = 12;
  float       m_persistance = 0.5f;
  float       m_lacunarity = 2.0f;
  glm::ivec2  m_noise_offset = {0,0};
  GLuint      m_seed = 1;
  float       m_height_scale = 1.75f;
  float       m_min_height = 0.f;
  float       m_texture_scale[8];
  float       m_min_tessellation_distance = 2.0f;
  float       m_max_tessellation_distance = 10.0f;
  int         m_snowness = 0;

  float             m_max_height_coef = 1.0f;
  float             m_min_height_coef = 0.0f;
  dbb::Bezier       m_interpolation_curve;
  bool              m_use_curve = false;
  bool              m_initialized = false;
  bool              m_auto_update = false;
  std::atomic<bool> m_update_textures = false;
  bool              m_apply_falloff = false;
  bool              m_use_normal_texture_pbr = false;
  float             m_fall_off_a = 3.f;
  float             m_fall_off_b = 2.2f;
  bool              m_apply_blur = false;
  float             m_blur_sigma = 1.0f;
  int32_t           m_blur_kernel_size = 5;
  int32_t           m_normal_sharpness = 10;

  eMainContextBase* m_game = nullptr;
  eModelManager*    m_modelManager = nullptr;
  eTextureManager*  m_texture_manager = nullptr;
  std::reference_wrapper<eOpenGlRenderPipeline> m_pipeline;
  IWindowImGui* m_imgui = nullptr;
};

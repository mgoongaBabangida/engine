#pragma once
#include <base/interfaces.h>
#include <glm/glm/gtc/noise.hpp>
#include <opengl_assets/Texture.h>

#include <vector>
#include <set>

class IWindowImGui;

struct TerrainType
{
  std::string name;
  float threshold;
  glm::vec3 color;
};

bool operator<(const TerrainType& _one, const TerrainType& _two);

class TerrainGeneratorTool : public IScript
{
public:

  TerrainGeneratorTool(IWindowImGui* _imgui);
  virtual ~TerrainGeneratorTool();

  virtual void Update(float _tick) override;

protected:
  void _GenerateNoiseMap(GLuint _width, GLuint _height, float _scale, GLuint octaves,
                         float persistance, float lacunarity, glm::vec2 offfset, GLuint _seed);

  void _GenerateColorMap();

  std::vector<GLfloat>    m_noise_map;
  std::vector<glm::vec4>  m_color_map;
  Texture m_noise_texture;
  Texture m_color_texture;
  std::set<TerrainType> m_terrain_types;

  GLuint m_width = 100;
  GLuint m_height = 100;
  float  m_scale = 25.0f;
  GLuint m_octaves = 4;
  float m_persistance = 0.5f;
  float m_lacunarity = 2.0f;
  glm::vec2 m_noise_offset = {0.0f, 0.0f};
  GLuint m_seed = 1;
};

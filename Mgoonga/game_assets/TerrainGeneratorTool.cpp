#include "stdafx.h"

#include "TerrainGeneratorTool.h"
#include "ObjectFactory.h"
#include "MainContextBase.h"

#include <sdl_assets/ImGuiContext.h>
#include <opengl_assets/TerrainModel.h>
#include <opengl_assets/ModelManager.h>
#include <math/Random.h>

float InverseLerp(float xx, float yy, float value)
{
	return (value - xx) / (yy - xx);
}

TerrainGeneratorTool::TerrainGeneratorTool(eMainContextBase* _game, eModelManager* _modelManager, IWindowImGui* _imgui)
	: m_game(_game)
	, m_modelManager(_modelManager)
{
	m_terrain_types.insert({ "water", 0.4f, {0.0f, 0.0f, 0.8f} });
	m_terrain_types.insert({ "grass", 0.6f, {0.0f, 1.0f, 0.0f} });
	m_terrain_types.insert({ "mounten", 0.8f, {0.5f, 0.5f, 0.0f} });
	m_terrain_types.insert({ "snow", 1.0f, {1.0f, 1.0f, 1.0f} });

	m_noise_map.resize(m_width * m_height);
	_GenerateNoiseMap(m_width, m_height, m_scale, m_octaves, m_persistance, m_lacunarity, m_noise_offset, m_seed);
	m_noise_texture.TextureFromBuffer<GLfloat>(&m_noise_map[0], m_width, m_height, GL_RED);

	m_color_map.resize(m_width * m_height);
	_GenerateColorMap();
	m_color_texture.TextureFromBuffer<GLfloat>(&m_color_map[0].x, m_width, m_height, GL_RGBA);

	//TERRAIN
	std::unique_ptr<TerrainModel> terrainModel = m_modelManager->CloneTerrain("simple");
	terrainModel->initialize(&m_color_texture,
													 &m_color_texture,
													 &Texture::GetTexture1x1(BLUE),
													 &m_noise_texture,
													 true);

	//OBJECTS
	ObjectFactoryBase factory;
	m_terrain_pointer = terrainModel.get();

	m_terrain = factory.CreateObject(std::shared_ptr<IModel>(terrainModel.release()), eObject::RenderType::PHONG, "Terrain");
	m_terrain->SetName("Procedural Terrain");
	m_terrain->GetTransform()->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	m_terrain->GetTransform()->setTranslation(glm::vec3(0.0f, 1.6f, 0.0f));
	//m_terrain->SetPickable(false);
	m_game->AddObject(m_terrain);

	std::function<void()> switch_lod__callback = [this]()
	{
		if(m_terrain_pointer->getMeshes()[0]->LODInUse() == 1)
			m_terrain_pointer->getMeshes()[0]->SwitchLOD(2);
		else
			m_terrain_pointer->getMeshes()[0]->SwitchLOD(1);
	};

	std::function<void()> render_mode__callback = [this]()
	{
		if (m_terrain_pointer->getMeshes()[0]->GetRenderMode() == MyMesh::RenderMode::DEFAULT)
			m_terrain_pointer->getMeshes()[0]->SetRenderMode(MyMesh::RenderMode::WIREFRAME);
		else
			m_terrain_pointer->getMeshes()[0]->SetRenderMode(MyMesh::RenderMode::DEFAULT);
	};

	_imgui->Add(TEXTURE, "Noise texture", (void*)m_noise_texture.id);
	_imgui->Add(SLIDER_INT, "Noise width", &m_width);
	_imgui->Add(SLIDER_INT, "Noise height", &m_height);
	_imgui->Add(SLIDER_FLOAT, "Scale", &m_scale);
	_imgui->Add(SLIDER_INT, "Octaves", &m_octaves);
	_imgui->Add(SLIDER_FLOAT, "Persistance", &m_persistance);
	_imgui->Add(SLIDER_FLOAT, "Lacunarity", &m_lacunarity);
	_imgui->Add(SLIDER_FLOAT, "Offset X", &m_noise_offset[0]);
	_imgui->Add(SLIDER_FLOAT, "Offset Y", &m_noise_offset[1]);
	_imgui->Add(SLIDER_INT, "Seed", &m_seed);
	_imgui->Add(BUTTON, "Switch LOD", (void*)&switch_lod__callback);
	_imgui->Add(BUTTON, "Render mode", (void*)&render_mode__callback);
	_imgui->Add(TEXTURE, "Color texture", (void*)m_color_texture.id);
}

TerrainGeneratorTool::~TerrainGeneratorTool()
{
}

//-----------------------------------------------------------------------------
void TerrainGeneratorTool::Update(float _tick)
{
	static float last_scale = m_scale;
	static float last_persistance = m_persistance;
	static float last_lacunarirty = m_lacunarity;
	static glm::vec2 noise_offset = { 0.0f, 0.0f };
	static GLuint seed = 1;

	if (m_noise_texture.mTextureWidth		!= m_width ||
			m_noise_texture.mTextureHeight	!= m_height ||
			last_scale											!= m_scale ||
			last_persistance								!= m_persistance ||
			last_lacunarirty								!= m_lacunarity ||
			noise_offset										!= m_noise_offset ||
			seed														!= m_seed)
	{
		m_noise_map.resize(m_width * m_height);
		_GenerateNoiseMap(m_width, m_height, m_scale, m_octaves, m_persistance, m_lacunarity, m_noise_offset, m_seed);

		//update noise texture
		m_noise_texture.mTextureWidth = m_width;
		m_noise_texture.mTextureHeight = m_height;
		glBindTexture(GL_TEXTURE_2D, m_noise_texture.id);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RED, m_width, m_height);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_FLOAT, &m_noise_map[0]);
		glBindTexture(GL_TEXTURE_2D, 0);

		//update color texture
		_GenerateColorMap();
		m_color_texture.TextureFromBuffer<GLfloat>(&m_color_map[0].x, m_width, m_height, GL_RGBA);

		//reset the mesh
		m_terrain_pointer->initialize(&m_color_texture,
																	&m_color_texture,
																	&Texture::GetTexture1x1(BLUE),
																	&m_noise_texture,
																	true);
	}
	last_scale = m_scale;
	last_persistance = m_persistance;
	last_lacunarirty = m_lacunarity;
	noise_offset = m_noise_offset;
	seed = m_seed;
}

//-----------------------------------------------------------------------------
void TerrainGeneratorTool::_GenerateNoiseMap(GLuint _width, GLuint _height, float _scale, GLuint _octaves,
																						 float _persistance, float _lacunarity, glm::vec2 _offset, GLuint _seed)
{
	std::vector<glm::vec2> octaveOffsets(_octaves);
	for (uint32_t i = 0; i < _octaves; ++i)
	{
		float offsetX = math::Random::RandomFloat(-10'000.0f, 10'000.0f, _seed) + _offset.x;
		float offsetY = math::Random::RandomFloat(-10'000.0f, 10'000.0f, _seed) + _offset.y;
		octaveOffsets[i] = { offsetX , offsetY };
	}

	if (_scale <= 0.0f)
		_scale = 0.0001f;
	if (_width < 1)
		_width = 1;
	if (_height < 1)
		_height = 1;
	if (_lacunarity < 1.0f)
		_lacunarity = 1.0f;
	if (_octaves < 1)
		_octaves = 1;

	float minHeight = 100'000.0f; // float max
	float maxHeight = -100'000.0f; // float min

	float halfWidth = _width / 2;
	float halfHeight = _height / 2;

	for (uint32_t row = 0; row < _height; ++row)
	{
		for (uint32_t col = 0; col < _width; ++col)
		{
			float amplitude = 1.0f;
			float frequency = 1.0f;
			float noiseHeight = 0.0f;

			for (uint32_t i = 0; i < _octaves; ++i)
			{
				float sampleX = (col - halfWidth + octaveOffsets[i].x) / _scale * frequency;
				float sampleY = (row - halfHeight + octaveOffsets[i].y) / _scale * frequency;
				float perlinValue = glm::perlin(glm::vec2{sampleX,sampleY}) * 2.0f - 1.0f;
				noiseHeight += perlinValue * amplitude;

				amplitude *= _persistance;
				frequency *= _lacunarity;
			}

			if (noiseHeight > maxHeight)
				maxHeight = noiseHeight;
			if (noiseHeight < minHeight)
				minHeight = noiseHeight;
			m_noise_map[row * _width + col] = noiseHeight;
		}
	}

	for (auto& val : m_noise_map)
		val = InverseLerp(maxHeight, minHeight, val);
}

//-----------------------------------------------------------------------------
void TerrainGeneratorTool::_GenerateColorMap()
{
	m_color_map.resize(m_noise_map.size());
	for (uint32_t row = 0; row < m_height; ++row)
	{
		for (uint32_t col = 0; col < m_width; ++col)
		{
			float height = m_noise_map[row * m_width + col];
			for (const auto& type : m_terrain_types)
			{
				if (height <= type.threshold)
				{
					m_color_map[row * m_width + col] = glm::vec4{ type.color, 1.0f };
					break;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
bool operator<(const TerrainType& _one, const TerrainType& _two)
{
	return _one.threshold < _two.threshold;
}

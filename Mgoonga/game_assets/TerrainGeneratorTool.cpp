#include "stdafx.h"

#include "TerrainGeneratorTool.h"
#include "ObjectFactory.h"
#include "MainContextBase.h"

#include <sdl_assets/ImGuiContext.h>
#include <opengl_assets/TerrainModel.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/openglrenderpipeline.h>
#include <math/Random.h>

float InverseLerp(float xx, float yy, float value)
{
	return (value - xx) / (yy - xx);
}

TerrainGeneratorTool::TerrainGeneratorTool(eMainContextBase* _game,
																					 eModelManager* _modelManager,
																					 eTextureManager* _texManager,
																					 eOpenGlRenderPipeline& _pipeline,
																					 IWindowImGui* _imgui)
	: m_game(_game)
	, m_modelManager(_modelManager)
	, m_texture_manager(_texManager)
	, m_pipeline(_pipeline)
	, m_imgui(_imgui)
{
	for (auto& s : m_texture_scale)
		s = 9.0f;
}

//------------------------------------------------------------------------------------------------
void TerrainGeneratorTool::Initialize()
{
	m_terrain_types.insert({ "water",		0.0f, 0.4f, {0.0f, 0.0f, 0.8f} });
	m_terrain_types.insert({ "grass",		0.4f,	0.6f, {0.0f, 1.0f, 0.0f} });
	m_terrain_types.insert({ "mounten", 0.6f, 0.8f, {0.5f, 0.5f, 0.0f} });
	m_terrain_types.insert({ "snow",		0.8f, 1.0f, {1.0f, 1.0f, 1.0f} });

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
		true,
		m_height_scale);

	//OBJECTS
	ObjectFactoryBase factory;
	m_terrain_pointer = terrainModel.get();

	m_terrain = factory.CreateObject(std::shared_ptr<IModel>(terrainModel.release()), eObject::RenderType::PHONG, "Terrain");
	m_terrain->SetName("Procedural Terrain");
	m_terrain->GetTransform()->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	m_terrain->GetTransform()->setTranslation(glm::vec3(0.0f, 1.6f, 0.0f));
	m_terrain->SetTextureBlending(true);
	m_terrain_pointer->setAlbedoTextureArray(m_texture_manager->Find("terrain_albedo_array_lague"));
	//m_terrain->SetPickable(false);
	m_game->AddObject(m_terrain);

	//SET UNIFORMS
	m_pipeline.get().SetUniformData("class eMainRender", "min_height", 0.0f);
	m_pipeline.get().SetUniformData("class eMainRender", "max_height", m_height_scale);
	m_pipeline.get().SetUniformData("class eMainRender", "color_count", m_terrain_types.size());

	int counter = 0;
	for (const auto& type : m_terrain_types)
	{
		m_pipeline.get().SetUniformData("class eMainRender",
																	  "base_start_heights["+ std::to_string(counter) +"]",
																		type.threshold_start);

		m_pipeline.get().SetUniformData("class eMainRender",
			"textureScale[" + std::to_string(counter) + "]",
			m_texture_scale[counter]);
		++counter;
	}
	m_pipeline.get().SetUniformData("class eMainRender",
		"base_start_heights[" + std::to_string(counter) + "]",
		1.0f);

	std::function<void()> switch_lod__callback = [this]()
	{
		if (m_terrain_pointer->getMeshes()[0]->LODInUse() == 1)
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

	std::function<void()> texturing__callback = [this]()
	{
		m_terrain->SetTextureBlending(!m_terrain->IsTextureBlending());
	};

	m_imgui->Add(TEXTURE, "Noise texture", (void*)m_noise_texture.id);
	m_imgui->Add(SLIDER_INT, "Noise width", &m_width);
	m_imgui->Add(SLIDER_INT, "Noise height", &m_height);
	m_imgui->Add(SLIDER_FLOAT, "Scale", &m_scale);
	m_imgui->Add(SLIDER_INT, "Octaves", &m_octaves);
	m_imgui->Add(SLIDER_FLOAT, "Persistance", &m_persistance);
	m_imgui->Add(SLIDER_FLOAT, "Lacunarity", &m_lacunarity);
	m_imgui->Add(SLIDER_FLOAT, "Offset X", &m_noise_offset[0]);
	m_imgui->Add(SLIDER_FLOAT, "Offset Y", &m_noise_offset[1]);
	m_imgui->Add(SLIDER_INT, "Seed", &m_seed);
	m_imgui->Add(SLIDER_FLOAT, "Height Scale", &m_height_scale);
	m_imgui->Add(SLIDER_FLOAT, "Texture Scale 0", &m_texture_scale[0]);
	m_imgui->Add(SLIDER_FLOAT, "Texture Scale 1", &m_texture_scale[1]);
	m_imgui->Add(SLIDER_FLOAT, "Texture Scale 2", &m_texture_scale[2]);
	m_imgui->Add(SLIDER_FLOAT, "Texture Scale 3", &m_texture_scale[3]);
	// @todo add update button alternative to auto update
	m_imgui->Add(BUTTON, "Switch LOD", (void*)&switch_lod__callback);
	m_imgui->Add(BUTTON, "Render mode", (void*)&render_mode__callback);
	m_imgui->Add(BUTTON, "Texturing", (void*)&texturing__callback);
	m_imgui->Add(TEXTURE, "Color texture", (void*)m_color_texture.id);
}

//-----------------------------------------------------------------------------
TerrainGeneratorTool::~TerrainGeneratorTool()
{
	m_color_texture.freeTexture();
	m_noise_texture.freeTexture();
}

//-----------------------------------------------------------------------------
void TerrainGeneratorTool::Update(float _tick)
{
	static float last_scale = m_scale;
	static float last_persistance = m_persistance;
	static float last_lacunarirty = m_lacunarity;
	static glm::vec2 noise_offset = { 0.0f, 0.0f };
	static GLuint seed = 1;
	static float last_height_scale = m_height_scale;

	static float last_texture_scales0 = m_texture_scale[0];
	static float last_texture_scales1 = m_texture_scale[1];
	static float last_texture_scales2 = m_texture_scale[2];
	static float last_texture_scales3 = m_texture_scale[3];

	if (last_texture_scales0 != m_texture_scale[0] ||
		  last_texture_scales1 != m_texture_scale[1] ||
		  last_texture_scales2 != m_texture_scale[2] ||
		  last_texture_scales3 != m_texture_scale[3] )
	{
		m_pipeline.get().SetUniformData("class eMainRender", "textureScale[0]", m_texture_scale[0]);
		m_pipeline.get().SetUniformData("class eMainRender", "textureScale[1]", m_texture_scale[1]);
		m_pipeline.get().SetUniformData("class eMainRender", "textureScale[2]", m_texture_scale[2]);
		m_pipeline.get().SetUniformData("class eMainRender", "textureScale[3]", m_texture_scale[3]);
	}

	if (m_noise_texture.mTextureWidth		!= m_width ||
			m_noise_texture.mTextureHeight	!= m_height ||
			last_scale											!= m_scale ||
			last_persistance								!= m_persistance ||
			last_lacunarirty								!= m_lacunarity ||
			noise_offset										!= m_noise_offset ||
			seed														!= m_seed ||
			last_height_scale								!= m_height_scale)
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
																	true,
																	m_height_scale);

		//update uniforms
		m_pipeline.get().SetUniformData("class eMainRender", "max_height", m_height_scale);
		m_pipeline.get().SetUniformData("class eMainRender", "textureScale[0]", m_texture_scale[0]);
	}

	last_scale = m_scale;
	last_persistance = m_persistance;
	last_lacunarirty = m_lacunarity;
	noise_offset = m_noise_offset;
	seed = m_seed;
	last_height_scale = m_height_scale;
	last_texture_scales0 = m_texture_scale[0];
	last_texture_scales1 = m_texture_scale[1];
	last_texture_scales2 = m_texture_scale[2];
	last_texture_scales3 = m_texture_scale[3];
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
				if (height <= type.threshold_finish)
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
	return _one.threshold_start < _two.threshold_start;
}

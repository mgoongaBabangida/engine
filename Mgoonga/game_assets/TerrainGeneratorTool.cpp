#include "stdafx.h"

#include "TerrainGeneratorTool.h"
#include "ObjectFactory.h"
#include "MainContextBase.h"

#include <sdl_assets/ImGuiContext.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/openglrenderpipeline.h>
#include <math/Random.h>
#include <math/BoxCollider.h>

#include "BezierCurveUIController.h"

float InverseLerp(float xx, float yy, float value)
{
	return (value - xx) / (yy - xx);
}

//------------------------------------------------------------------------------------------------
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
		s = 3.0f;
}

//------------------------------------------------------------------------------------------------
void TerrainGeneratorTool::Initialize()
{
	m_terrain_types.insert({ "water",		0.0f, 0.4f, {0.0f, 0.0f, 0.8f} });
	m_terrain_types.insert({ "grass",		0.4f,	0.6f, {0.0f, 1.0f, 0.0f} });
	m_terrain_types.insert({ "mounten", 0.6f, 0.8f, {0.5f, 0.5f, 0.0f} });
	m_terrain_types.insert({ "snow",		0.8f, 1.0f, {1.0f, 1.0f, 1.0f} });

	//@todo needs to be regenerated when resized
	m_falloff_map.resize(m_width * m_height);
	_GenerateFallOffMap();

	m_noise_map.resize(m_width * m_height);
	m_octaves_buffer.resize(m_octaves);
	for (auto& octave_buffer : m_octaves_buffer)
		octave_buffer.resize(m_width * m_height);

	_GenerateNoiseMap(m_width, m_height, m_scale, m_octaves, m_persistance, m_lacunarity, m_noise_offset, m_seed);
	m_noise_texture.TextureFromBuffer<GLfloat>(&m_noise_map[0], m_width, m_height, GL_RED);

	m_color_map.resize(m_width * m_height);
	_GenerateColorMap();
	m_color_texture.TextureFromBuffer<GLfloat>(&m_color_map[0].x, m_width, m_height, GL_RGBA);

	//TERRAIN
	std::unique_ptr<TerrainModel> terrainModel = m_modelManager->CloneTerrain("simple");

	//OBJECTS
	ObjectFactoryBase factory;
	m_terrain_pointer = terrainModel.get();

	m_terrain = factory.CreateObject(std::shared_ptr<IModel>(terrainModel.release()), eObject::RenderType::PHONG, "Terrain");
	m_terrain->SetCollider(new BoxCollider);
	m_terrain->SetName("Procedural Terrain");
	//m_terrain->GetTransform()->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	//m_terrain->GetTransform()->setTranslation(glm::vec3(0.0f, 1.6f, 0.0f));
	m_terrain->SetTextureBlending(true);
	m_terrain_pointer->setAlbedoTextureArray(m_texture_manager->Find("terrain_albedo_array_lague"));
	//m_terrain->SetPickable(false);
	m_game->AddObject(m_terrain);

	//@todo delete duplication
	int counter = 0;
	for (const auto& type : m_terrain_types)
	{
		m_pipeline.get().SetUniformData("class ePhongRender",
																	  "base_start_heights["+ std::to_string(counter) +"]",
																		type.threshold_start);

		m_pipeline.get().SetUniformData("class ePhongRender",
			"textureScale[" + std::to_string(counter) + "]",
			m_texture_scale[counter]);

		++counter;
	}

	m_pipeline.get().SetUniformData("class ePhongRender",
		"base_start_heights[" + std::to_string(counter) + "]",
		1.0f);

	counter = 0;
	for (const auto& type : m_terrain_types)
	{
		m_pipeline.get().SetUniformData("class eTerrainTessellatedRender",
			"base_start_heights[" + std::to_string(counter) + "]",
			type.threshold_start);

		m_pipeline.get().SetUniformData("class eTerrainTessellatedRender",
			"textureScale[" + std::to_string(counter) + "]",
			m_texture_scale[counter]);

		++counter;
	}

	m_pipeline.get().SetUniformData("class eTerrainTessellatedRender",
		"base_start_heights[" + std::to_string(counter) + "]",
		1.0f);

	std::function<void()> switch_lod__callback = [this]()
	{
		for (auto& mesh : m_terrain_pointer->getMeshes())
		{
			if (mesh->LODInUse() == 1)
				mesh->SwitchLOD(2);
			else if (mesh->LODInUse() == 2)
				mesh->SwitchLOD(3);
			else
				mesh->SwitchLOD(1);
		}
	};

	std::function<void()> render_mode__callback = [this]()
	{
		for (auto& mesh : m_terrain_pointer->getMeshes())
		{
			if (mesh->GetRenderMode() == MyMesh::RenderMode::DEFAULT)
				mesh->SetRenderMode(MyMesh::RenderMode::WIREFRAME);
			else
				mesh->SetRenderMode(MyMesh::RenderMode::DEFAULT);
		}
	};

	std::function<void()> texturing__callback = [this]()
	{
		m_terrain->SetTextureBlending(!m_terrain->IsTextureBlending());
	};

	std::function<void()> tessellation__callback = [this]()
	{
		if (m_terrain->GetRenderType() != eObject::RenderType::TERRAIN_TESSELLATION)
		{
			m_terrain_pointer->EnableTessellation(true);
			m_terrain->SetRenderType(eObject::RenderType::TERRAIN_TESSELLATION);
		}
		else
		{
			m_terrain_pointer->EnableTessellation(false);
			m_terrain->SetRenderType(eObject::RenderType::PHONG);
		}
	};

	std::function<void()> update__callback = [this]()
	{
		_AddCurrentMesh();
	};

	static std::function<void(int, int*&)> posX__callback = [this](int _new_value, int*& _data)
	{
		if (_data == nullptr)
			_data = &m_cur_pos_X;
		else
			m_cur_pos_X = _new_value;
	};

	static std::function<void(int, int*&)> posY__callback = [this](int _new_value, int*& _data)
	{
		if (_data == nullptr)
			_data = &m_cur_pos_Y;
		else
			m_cur_pos_Y = _new_value;
	};

	m_imgui->Add(TEXTURE, "Noise texture", (void*)m_noise_texture.id);
	m_imgui->Add(SPIN_BOX, "Position X", (void*)&posX__callback);
	m_imgui->Add(SPIN_BOX, "Position Y", (void*)&posY__callback);
	m_imgui->Add(BUTTON, "Add or Update", (void*)&update__callback);
	m_imgui->Add(SLIDER_INT, "Noise width", &m_width);
	m_imgui->Add(SLIDER_INT, "Noise height", &m_height);
	m_imgui->Add(SLIDER_FLOAT_LARGE, "Scale", &m_scale);
	m_imgui->Add(SLIDER_INT_NERROW, "Octaves", &m_octaves);
	m_imgui->Add(SLIDER_FLOAT, "Persistance", &m_persistance);
	m_imgui->Add(SLIDER_FLOAT, "Lacunarity", &m_lacunarity);
	m_imgui->Add(SLIDER_FLOAT_LARGE, "Offset X", &m_noise_offset[0]);
	m_imgui->Add(SLIDER_FLOAT_LARGE, "Offset Y", &m_noise_offset[1]);
	m_imgui->Add(SLIDER_INT, "Seed", &m_seed);
	m_imgui->Add(SLIDER_FLOAT, "Height Scale", &m_height_scale);
	m_imgui->Add(SLIDER_FLOAT, "Texture Scale 0", &m_texture_scale[0]);
	m_imgui->Add(SLIDER_FLOAT, "Texture Scale 1", &m_texture_scale[1]);
	m_imgui->Add(SLIDER_FLOAT, "Texture Scale 2", &m_texture_scale[2]);
	m_imgui->Add(SLIDER_FLOAT, "Texture Scale 3", &m_texture_scale[3]);
	m_imgui->Add(CHECKBOX, "Use Fall Off", (void*)&m_apply_falloff);
	m_imgui->Add(BUTTON, "Switch LOD", (void*)&switch_lod__callback);
	m_imgui->Add(BUTTON, "Render mode", (void*)&render_mode__callback);
	m_imgui->Add(BUTTON, "Texturing", (void*)&texturing__callback);
	m_imgui->Add(BUTTON, "Tessellation", (void*)&tessellation__callback);
	m_imgui->Add(TEXTURE, "Color texture", (void*)m_color_texture.id);

	dbb::Bezier bezier;
	bezier.p0 = { -0.85f, -0.75f, 0.0f };
	bezier.p1 = { -0.45f, -0.33f, 0.0f };
	bezier.p2 = { 0.17f,  0.31f, 0.0f };
	bezier.p3 = { 0.55f,  0.71f, 0.0f };
	m_interpolation_curve = bezier;

	std::function<void()> create_bezier_callback = [this]()
	{
		ObjectFactoryBase factory;
		shObject bezier_model = factory.CreateObject(std::make_shared<BezierCurveModel>(new BezierCurveMesh(m_interpolation_curve, /*2d*/true)), eObject::RenderType::BEZIER_CURVE);
		m_game->AddObject(bezier_model);

		for (int i = 0; i < 4; ++i)
		{
			shObject pbr_sphere = factory.CreateObject(m_modelManager->Find("sphere_red"), eObject::RenderType::PBR, "SphereBezierPBR " + std::to_string(i));
			bezier_model->GetChildrenObjects().push_back(pbr_sphere);
			pbr_sphere->Set2DScreenSpace(true);
		}
		auto* script = new BezierCurveUIController(m_game, bezier_model, 0.02f, m_texture_manager->Find("pseudo_imgui"));
		script->ToolFinished.Subscribe([this](const dbb::Bezier& _bezier) { m_interpolation_curve = _bezier; Update(0); });
		bezier_model->SetScript(script);
	};

	m_imgui->Add(CHECKBOX, "Use Curve", (void*)&m_use_curve);
	m_imgui->Add(BUTTON, "Interpolation Curve", (void*)&create_bezier_callback);

	m_initialized = true;

	m_terrain_pointer->SetCamera(&m_game->GetMainCamera());
	if (true)
	{
		_UpdateCurrentMesh();
	}
	else
	{
		m_auto_update = true;
		for (int x = -1; x <= 1; ++x)
			for (int y = -1; y <= 1; ++y)
		{
				m_cur_pos_X = x;
				m_cur_pos_Y = y;
				m_noise_offset.x = x * ((int)m_width);
				m_noise_offset.y = y * ((int)m_height);
				Update(0);
		}
		m_auto_update = false;
	}

	m_imgui->Add(TEXTURE, "Normal map", (void*)m_terrain_pointer->GetMaterial()->normal_texture_id);
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
	if (m_initialized)
	{
		static float last_scale = m_scale;
		static float last_persistance = m_persistance;
		static float last_lacunarirty = m_lacunarity;
		static glm::vec2 noise_offset = m_noise_offset;
		static GLuint octaves = m_octaves;
		static GLuint seed = m_seed;
		static float last_height_scale = m_height_scale;
		static bool use_falloff = false;
		static bool use_curve = false;

		static float last_texture_scales0 = m_texture_scale[0];
		static float last_texture_scales1 = m_texture_scale[1];
		static float last_texture_scales2 = m_texture_scale[2];
		static float last_texture_scales3 = m_texture_scale[3];

		if (last_texture_scales0 != m_texture_scale[0] ||
			last_texture_scales1 != m_texture_scale[1] ||
			last_texture_scales2 != m_texture_scale[2] ||
			last_texture_scales3 != m_texture_scale[3])
		{
			m_pipeline.get().SetUniformData("class ePhongRender", "textureScale[0]", m_texture_scale[0]);
			m_pipeline.get().SetUniformData("class ePhongRender", "textureScale[1]", m_texture_scale[1]);
			m_pipeline.get().SetUniformData("class ePhongRender", "textureScale[2]", m_texture_scale[2]);
			m_pipeline.get().SetUniformData("class ePhongRender", "textureScale[3]", m_texture_scale[3]);

			m_pipeline.get().SetUniformData("class eTerrainTessellatedRender", "textureScale[0]", m_texture_scale[0]);
			m_pipeline.get().SetUniformData("class eTerrainTessellatedRender", "textureScale[1]", m_texture_scale[1]);
			m_pipeline.get().SetUniformData("class eTerrainTessellatedRender", "textureScale[2]", m_texture_scale[2]);
			m_pipeline.get().SetUniformData("class eTerrainTessellatedRender", "textureScale[3]", m_texture_scale[3]);
		}

		if (m_noise_texture.mTextureWidth != m_width ||
				m_noise_texture.mTextureHeight != m_height ||
				last_scale != m_scale ||
				last_persistance != m_persistance ||
				last_lacunarirty != m_lacunarity ||
				noise_offset != m_noise_offset ||
				octaves != m_octaves ||
				seed != m_seed ||
				last_height_scale != m_height_scale ||
				use_falloff != m_apply_falloff ||
				use_curve != m_use_curve)
		{
			m_noise_map.resize(m_width * m_height);
			m_octaves_buffer.resize(m_octaves);
			for (auto& octave_buffer : m_octaves_buffer)
				octave_buffer.resize(m_width * m_height);

			_GenerateNoiseMap(m_width, m_height, m_scale, m_octaves, m_persistance, m_lacunarity, m_noise_offset, m_seed);

			//update noise texture
			m_noise_texture.mTextureWidth = m_width;
			m_noise_texture.mTextureHeight = m_height;
			m_noise_texture.mChannels = 1;
			glBindTexture(GL_TEXTURE_2D, m_noise_texture.id);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RED, m_width, m_height);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_FLOAT, &m_noise_map[0]);
			glBindTexture(GL_TEXTURE_2D, 0);
			m_noise_texture.saveToFile("noise_terrain.jpg", GL_TEXTURE_2D, GL_RED, GL_FLOAT);
			//update color texture
			_GenerateColorMap();
			m_color_texture.TextureFromBuffer<GLfloat>(&m_color_map[0].x, m_width, m_height, GL_RGBA);

			//reset the mesh
			if (m_auto_update)
				_AddCurrentMesh();
		}

		last_scale = m_scale;
		last_persistance = m_persistance;
		last_lacunarirty = m_lacunarity;
		noise_offset = m_noise_offset;
		octaves = m_octaves;
		seed = m_seed;
		last_height_scale = m_height_scale;
		last_texture_scales0 = m_texture_scale[0];
		last_texture_scales1 = m_texture_scale[1];
		last_texture_scales2 = m_texture_scale[2];
		last_texture_scales3 = m_texture_scale[3];
		use_falloff = m_apply_falloff;
		use_curve = m_use_curve;
	}
}

//-----------------------------------------------------------------------------
void TerrainGeneratorTool::_GenerateNoiseMap(GLuint _width, GLuint _height, float _scale, GLuint _octaves,
																						 float _persistance, float _lacunarity, glm::vec2 _offset, GLuint _seed)
{
	float maxPossibleHeight = 0;

	std::vector<glm::vec2> octaveOffsets(_octaves);
	float gamplitude = 1.f;
	for (uint32_t i = 0; i < _octaves; ++i)
	{
		float offsetX = math::Random::RandomFloat(-10'000.0f, 10'000.0f, _seed) + _offset.x;
		float offsetY = math::Random::RandomFloat(-10'000.0f, 10'000.0f, _seed) + _offset.y;
		octaveOffsets[i] = { offsetX , offsetY };

		maxPossibleHeight += gamplitude;
		gamplitude *= _persistance;
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

	// for seemless tiles need the same max min height interpretation
	static float minHeight = 100'000.0f; // float max
	static float maxHeight = -100'000.0f; // float min
	static bool update_max_heights = true;

	// launch master thread not to block the main thread
	/*std::function<bool()> main_func = [this, _octaves, _persistance, _lacunarity, _height, _width, _scale, 
																		 octaveOffsets]()->bool
	{*/
		float halfWidth = _width / 2;
		float halfHeight = _height / 2;

		float amplitude = 1.0f;
		float frequency = 1.0f;

		std::vector<std::future<bool>> m_tasks;
		// launch async task for every octave
		for (uint32_t octave = 0; octave < _octaves; ++octave)
		{
			std::function<bool()> func = [this, amplitude, frequency, halfWidth, halfHeight,
				octave, _height, _width, _scale, octaveOffsets]()->bool
			{
				for (uint32_t row = 0; row < _height; ++row)
				{
					for (uint32_t col = 0; col < _width; ++col)
					{
						float sampleX = (col - halfWidth + octaveOffsets[octave].x) / _scale * frequency;
						float sampleY = (row - halfHeight + octaveOffsets[octave].y) / _scale * frequency;
						float perlinValue = glm::perlin(glm::vec2{ sampleX, sampleY }) * 2.0f - 1.0f;
						m_octaves_buffer[octave][row * _width + col] = perlinValue * amplitude;
					}
				}
				return true;
			};
			m_tasks.emplace_back(std::async(func));
			amplitude *= _persistance;
			frequency *= _lacunarity;
		}

		//wait for the tasks
		for (auto& fut : m_tasks)
		{
			fut.get();
		}

		//sum up all the octaves
		for (uint32_t i = 0; i < _octaves; ++i)
			for (uint32_t row = 0; row < _height; ++row)
				for (uint32_t col = 0; col < _width; ++col)
					m_noise_map[row * _width + col] += m_octaves_buffer[i][row * _width + col];

		if (update_max_heights)
		{
			for (uint32_t i = 0; i < m_noise_map.size(); ++i)
			{
				if (m_noise_map[i] > maxHeight)
					maxHeight = m_noise_map[i];
				if (m_noise_map[i] < minHeight)
					minHeight = m_noise_map[i];
			}
		}

		dbb::Bezier interpolation_curve_normalized;
		interpolation_curve_normalized.p0 = { (m_interpolation_curve.p0.x + 1.0f) / 2.0f, (m_interpolation_curve.p0.y + 1.0f) / 2.0f, 0.0f }; // ( +1) /2 from -1 1 to 0 1
		interpolation_curve_normalized.p1 = { (m_interpolation_curve.p1.x + 1.0f) / 2.0f, (m_interpolation_curve.p1.y + 1.0f) / 2.0f, 0.0f };
		interpolation_curve_normalized.p2 = { (m_interpolation_curve.p2.x + 1.0f) / 2.0f, (m_interpolation_curve.p2.y + 1.0f) / 2.0f, 0.0f };
		interpolation_curve_normalized.p3 = { (m_interpolation_curve.p3.x + 1.0f) / 2.0f, (m_interpolation_curve.p3.y + 1.0f) / 2.0f, 0.0f };
		
		//noise values should be normalized
		for (auto& val : m_noise_map)
		{
			val = InverseLerp(maxHeight, minHeight, val);
			if (m_use_curve)
			{
				val = dbb::GetPoint(interpolation_curve_normalized, val).y;
			}
		}

		if (m_apply_falloff)
			for (int i = 0; i < m_noise_map.size(); ++i)
				m_noise_map[i] = glm::clamp(m_noise_map[i] - m_falloff_map[i], 0.f, 1.f);

		update_max_heights = false;
	/*	return true;
	};

	m_generat_noise_task = std::async(main_func);*/
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
void TerrainGeneratorTool::_UpdateCurrentMesh()
{
	if (m_terrain_pointer)
	{
		m_terrain_pointer->Initialize(&m_color_texture,
																	&m_color_texture,
																	&Texture::GetTexture1x1(BLUE),
																	&m_noise_texture,
																	true,
																	m_height_scale,
																	m_height_scale * m_max_height_coef);

		//update uniforms
		m_pipeline.get().SetUniformData("class ePhongRender", "min_height", 0.0f);
		m_pipeline.get().SetUniformData("class ePhongRender", "max_height", m_height_scale);
		m_pipeline.get().SetUniformData("class ePhongRender", "textureScale[0]", m_texture_scale[0]);

		m_pipeline.get().SetUniformData("class eTerrainTessellatedRender", "min_height", 0.0f);
		m_pipeline.get().SetUniformData("class eTerrainTessellatedRender", "max_height", m_height_scale);
		m_pipeline.get().SetUniformData("class eTerrainTessellatedRender", "textureScale[0]", m_texture_scale[0]);
	}
}

//-----------------------------------------------------------------------------
void TerrainGeneratorTool::_AddCurrentMesh()
{
	if (m_terrain_pointer)
	{
		m_terrain_pointer->AddOrUpdate(glm::ivec2(m_cur_pos_X, m_cur_pos_Y),
																	 m_noise_offset,
																	 &m_color_texture,
																	 &m_noise_texture,
																	 true,
																	 m_height_scale,
																	 m_height_scale * m_max_height_coef);

		//update uniforms
		m_pipeline.get().SetUniformData("class ePhongRender", "min_height", 0.0f);
		m_pipeline.get().SetUniformData("class ePhongRender", "max_height", m_height_scale);
		m_pipeline.get().SetUniformData("class ePhongRender", "textureScale[0]", m_texture_scale[0]);

		m_pipeline.get().SetUniformData("class eTerrainTessellatedRender", "min_height", 0.0f);
		m_pipeline.get().SetUniformData("class eTerrainTessellatedRender", "max_height", m_height_scale);
		m_pipeline.get().SetUniformData("class eTerrainTessellatedRender", "textureScale[0]", m_texture_scale[0]);
	}
}

//-----------------------------------------------------------------------------
void TerrainGeneratorTool::_GenerateFallOffMap()
{
	for (uint32_t row = 0; row < m_height; ++row)
	{
		for (uint32_t col = 0; col < m_width; ++col)
		{
			float x = row / (float)m_height * 2 - 1;
			float y = col / (float)m_width * 2 - 1;
			float value = glm::max(glm::abs(x), glm::abs(y));
			// function adjustment
			float a = 3.f, b = 2.2f;
			value = glm::pow(value, a) / (glm::pow(value, a) + glm::pow(b - b * value, a) );
			m_falloff_map[row * m_width + col] = value;
		}
	}
}

#include "stdafx.h"

#include "VolumetricCloudsTool.h"
#include "MainContextBase.h"
#include "ObjectFactory.h"
#include "ModelManagerYAML.h"

#include <sdl_assets/ImGuiContext.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/openglrenderpipeline.h>

#include "ObjectFactory.h"
#include "MainContextBase.h"

//----------------------------------------------------------------------------------------------------------
VolumetricCloudsTool::VolumetricCloudsTool(eMainContextBase* _game, eModelManager* _modelManager, eTextureManager* _texManager, eOpenGlRenderPipeline& _pipeline, IWindowImGui* _imgui)
	: m_game(_game)
	, m_modelManager(_modelManager)
	, m_texture_manager(_texManager)
	, m_pipeline(_pipeline)
	, m_imgui(_imgui)
{
}

//----------------------------------------------------------------------------------------------------------
VolumetricCloudsTool::~VolumetricCloudsTool()
{
}

//----------------------------------------------------------------------------------------------------------
void VolumetricCloudsTool::Update(float _tick)
{

}

//----------------------------------------------------------------------------------------------------------
void VolumetricCloudsTool::Initialize()
{
	ObjectFactoryBase factory;
	shObject cube = factory.CreateObject(m_game->GetModelManager()->Find("wall_cube"), eObject::RenderType::VOLUMETRIC, "VolumetricCube");
	m_game->AddObject(cube);

	std::function<void()> test_callback = []()
	{
	};
	m_imgui->Add(BUTTON, "Test", (void*)&test_callback);
	static auto id =  m_pipeline.get().GetComputeParticleSystem().id;
	m_imgui->Add(TEXTURE, "Noise 3D", (void*)id);
	m_imgui->Add(SLIDER_FLOAT_NERROW, "Noise 3D Z Debug", &m_pipeline.get().Noize3DZDebug());
	static std::function<void(int)> debug_octave_callback = [this](int _octave)
	{
		if (_octave >= 0 && _octave <= 4)
			m_pipeline.get().Noize3DOctaveDebug() = _octave;
	};
	m_imgui->Add(SPIN_BOX, "Debug octave", (void*)&debug_octave_callback);
	static std::function<void(int)> control_density = [this](int _value)
	{
		if(_value >= 0)
			m_pipeline.get().GetCloudDensity() = _value;
	};
	m_imgui->Add(SPIN_BOX, "Density", (void*)&control_density);
	m_imgui->Add(SLIDER_INT, "Absorption", &m_pipeline.get().GetCloudAbsorption());
	m_imgui->Add(SLIDER_FLOAT, "Perlin weight", &m_pipeline.get().GetCloudPerlinWeight());
	m_imgui->Add(SLIDER_FLOAT_3, "Cloud color", &m_pipeline.get().GetCloudColor());
	static std::function<void(int)> perlin_motion = [this](int _value)
	{
		if (_value >= 0)
			m_pipeline.get().GetCloudWorleyMotion() = _value;
	};
	m_imgui->Add(SPIN_BOX, "Perlin motion", (void*)&perlin_motion);
	static std::function<void(int)> worley_motion = [this](int _value)
	{
		if (_value >= 0)
			m_pipeline.get().GetCloudWorleyMotion() = _value;
	};
	m_imgui->Add(SPIN_BOX, "Worley motion", (void*)&worley_motion);
}

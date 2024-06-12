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
	static auto id =  m_pipeline.get().GetComputeParticleSystem().m_id;
	m_imgui->Add(TEXTURE, "Noise 3D", (void*)id);
	m_imgui->Add(SLIDER_FLOAT_NERROW, "Noise 3D Z Debug", &m_pipeline.get().Noize3DZDebug());

	static std::function<void(int, int*&)> debug_octave_callback = [this](int _octave, int*& _val)
	{
		static bool first_call = true;
		if (first_call)
		{
			*_val = m_pipeline.get().Noize3DOctaveDebug();
			first_call = false;
			return;
		}

		if (_octave >= 0 && _octave <= 4)
			m_pipeline.get().Noize3DOctaveDebug() = _octave;
	};
	m_imgui->Add(SPIN_BOX, "Debug octave", (void*)&debug_octave_callback);

	static std::function<void(int, int*&)> octave_size_callback = [this](int _size, int*& _val)
	{
		static bool first_call = true;
		if (first_call)
		{
			*_val = m_pipeline.get().GetWorleyOctaveSizeOne();
			first_call = false;
			return;
		}

		auto cur_octave = m_pipeline.get().Noize3DOctaveDebug();
		if (cur_octave == 0)
			m_pipeline.get().GetWorleyOctaveSizeOne() = _size;
		else if(cur_octave == 1)
			m_pipeline.get().GetWorleyOctaveSizeTwo() = _size;
		else if(cur_octave == 2)
			m_pipeline.get().GetWorleyOctaveSizeThree() = _size;
	};
	m_imgui->Add(SPIN_BOX, "Current Octave Size", (void*)&octave_size_callback);
	m_imgui->Add(SLIDER_INT, "Noise Gamma", &m_pipeline.get().GetWorleyNoiseGamma());
	std::function<void(void)> redo_noise_callback = [this]() { m_pipeline.get().RedoWorleyNoise(); };
	m_imgui->Add(BUTTON, "Redo Noise", &redo_noise_callback);

	static std::function<void(int, int*&)> control_density = [this](int _value, int*& _vo)
	{
		static bool first_call = true;
		if (first_call)
		{
			*_vo = m_pipeline.get().GetCloudDensity();
			first_call = false;
			return;
		}

		if(_value >= 0)
			m_pipeline.get().GetCloudDensity() = _value;
	};
	m_imgui->Add(SPIN_BOX, "Density", (void*)&control_density);

	m_imgui->Add(SLIDER_INT, "Absorption", &m_pipeline.get().GetCloudAbsorption());
	m_imgui->Add(SLIDER_FLOAT, "Perlin weight", &m_pipeline.get().GetCloudPerlinWeight());
	m_imgui->Add(SLIDER_FLOAT_NERROW, "G Value", &m_pipeline.get().GetCloudGParam());

	static std::function<void(int, int*&)> perlin_motion = [this](int _value, int*& _vo)
	{
		static bool first_call = true;
		if (first_call)
		{
			*_vo = m_pipeline.get().GetCloudPerlinMotion();
			first_call = false;
			return;
		}

		if (_value > 0)
			m_pipeline.get().GetCloudPerlinMotion() = _value;
	};
	m_imgui->Add(SPIN_BOX, "Perlin motion", (void*)&perlin_motion);
	static std::function<void(int, int*&)> worley_motion = [this](int _value, int*& _vo)
	{
		static bool first_call = true;
		if (first_call)
		{
			*_vo = m_pipeline.get().GetCloudWorleyMotion();
			first_call = false;
			return;
		}

		if (_value > 0)
			m_pipeline.get().GetCloudWorleyMotion() = _value;
	};
	m_imgui->Add(SPIN_BOX, "Worley motion", (void*)&worley_motion);

	m_imgui->Add(CHECKBOX, "Apply Powder", &m_pipeline.get().GetApplyPowder());
	m_imgui->Add(SLIDER_FLOAT_NERROW, "Alpha threhsold", &m_pipeline.get().GetAlphathreshold());
	m_imgui->Add(CHECKBOX, "Silver lining", &m_pipeline.get().GetCloudSilverLining());
	m_imgui->Add(SLIDER_FLOAT_NERROW, "Silver lining density", &m_pipeline.get().GetSilverLiningDensity());
	m_imgui->Add(SLIDER_INT_NERROW, "Silver lining strength", &m_pipeline.get().GetSilverLiningStrength());
	m_imgui->Add(SLIDER_FLOAT_3, "Cloud color", &m_pipeline.get().GetCloudColor());
	m_imgui->Add(SLIDER_FLOAT_3, "Noise scale", &m_pipeline.get().GetNoiseScale());
}

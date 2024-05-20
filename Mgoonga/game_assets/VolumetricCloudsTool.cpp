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
}

#include "stdafx.h"

#include "ParticleSystemToolController.h"
#include "MainContextBase.h"
#include "ObjectFactory.h"
#include "BezierCurveUIController.h"

#include <math/ParticleSystem.h>
#include <math/Bezier.h>

#include <sdl_assets/ImGuiContext.h>

#include <opengl_assets/TextureManager.h>
#include <opengl_assets/SoundManager.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/Sound.h>
#include <opengl_assets/OpenGlRenderPipeline.h>
#include <opengl_assets/MyModel.h>

//---------------------------------------------------------------------------
ParticleSystemToolController::ParticleSystemToolController(eMainContextBase* _game,
																													IWindowImGui* _imgui,
																													eModelManager* _modelManager,
																													eTextureManager* _texManager,
																													eSoundManager* _soundManager,
																													eOpenGlRenderPipeline& _pipeline)
{
	//Particle System Tool
	m_psystem = std::make_shared<ParticleSystem>(50, 1.0f/50.0f, 0, 10'000, glm::vec3(0.5f, 3.0f, -2.5f),
																							_texManager->Find("Tatlas2"),
																							_soundManager->GetSound("shot_sound"),
																							_texManager->Find("Tatlas2")->numberofRows);

	std::function<void()> emit_ps_callback = [this, &_pipeline]()
	{
		m_psystem->Reset();
		_pipeline.AddParticleSystem(m_psystem);
	};

	_imgui->Add(PARTICLE_SYSTEM, "Particle system", (void*)&m_psystem);

	std::function<void()> create_bezier_callback = [this, _game, _texManager, _modelManager]()
	{
		dbb::Bezier bezier;
		bezier.p0 = { -0.85f, -0.75f, 0.0f };
		bezier.p1 = { -0.45f, -0.33f, 0.0f };
		bezier.p2 = { 0.17f,  0.31f, 0.0f };
		bezier.p3 = { 0.55f,  0.71f, 0.0f };

		ObjectFactoryBase factory;
		shObject bezier_model = factory.CreateObject(std::make_shared<BezierCurveModel>(new BezierCurveMesh(bezier, /*2d*/true)), eObject::RenderType::BEZIER_CURVE);
		_game->AddObject(bezier_model);

		for (int i = 0; i < 4; ++i)
		{
			shObject pbr_sphere = factory.CreateObject(_modelManager->Find("sphere_red"), eObject::RenderType::PBR, "SphereBezierPBR " + std::to_string(i));
			bezier_model->GetChildrenObjects().push_back(pbr_sphere);
			pbr_sphere->Set2DScreenSpace(true);
		}
		auto* script = new BezierCurveUIController(_game, bezier_model, 0.02f, _texManager->Find("pseudo_imgui"));
		script->ToolFinished.Subscribe([this](const dbb::Bezier& _bezier) { m_psystem->SetSizeBezier({ _bezier.p0,_bezier.p1,_bezier.p2, _bezier.p3 }); });
		bezier_model->SetScript(script);
	};
	_imgui->Add(BUTTON, "Size Curve", (void*)&create_bezier_callback);

	_imgui->Add(BUTTON, "Emit", (void*)&emit_ps_callback);
}

ParticleSystemToolController::~ParticleSystemToolController()
{
	m_psystem.reset();
}

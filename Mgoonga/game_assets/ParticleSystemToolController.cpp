#include "ParticleSystemToolController.h"

#include <sdl_assets/ImGuiContext.h>
#include <math/ParticleSystem.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/SoundManager.h>
#include <opengl_assets/Sound.h>
#include <opengl_assets/OpenGlRenderPipeline.h>

//---------------------------------------------------------------------------
ParticleSystemToolController::ParticleSystemToolController(IWindowImGui* _imgui,
																													eTextureManager* _texManager,
																													eSoundManager* _soundManager,
																													eOpenGlRenderPipeline& _pipeline)
{
	//Particle System Tool
	m_psystem = std::make_shared<ParticleSystem>(50, 1.0f / 50.0f, 0, 10000, glm::vec3(0.5f, 3.0f, -2.5f),
		_texManager->Find("Tatlas2"),
		_soundManager->GetSound("shot_sound"),
		_texManager->Find("Tatlas2")->numberofRows);

	std::function<void()> emit_ps_callback = [this, &_pipeline]()
	{
		m_psystem->Start();
		_pipeline.AddParticleSystem(m_psystem);
	};

	_imgui->Add(PARTICLE_SYSTEM, "Particle system", (void*)&m_psystem);
	_imgui->Add(BUTTON, "Emit", (void*)&emit_ps_callback);
}
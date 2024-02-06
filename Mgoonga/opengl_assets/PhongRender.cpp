#include "stdafx.h"
#include "PhongRender.h"
#include <math/Transform.h>
#include <math/Rigger.h>

//---------------------------------------------------------------------------------
ePhongRender::ePhongRender(const std::string& vS, const std::string& wave_vS, const std::string& fS,
	std::unique_ptr<TerrainModel> model, const Texture* tex)
: matrices(MAX_BONES)
{
	mainShader.installShaders(vS.c_str(), fS.c_str()); //main pass
	glUseProgram(mainShader.ID());

	//Uniform Locs
	BonesMatLocation					= glGetUniformLocation(mainShader.ID(), "gBones");
	LightingIndexDirectional	= glGetSubroutineIndex(mainShader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongDirectionalSpecDif");
	LightingIndexPoint				= glGetSubroutineIndex(mainShader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongPointSpecDif");
	LightingIndexSpot					= glGetSubroutineIndex(mainShader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongFlashSpecDif");

	mainShader.SetUniformData("Fog.maxDist", 40.0f);
	mainShader.SetUniformData("Fog.minDist", 20.0f);
	mainShader.SetUniformData("Fog.color", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	mainShader.SetUniformData("Fog.fog_on", true);
	mainShader.SetUniformData("Fog.density", 0.007f);
	mainShader.SetUniformData("Fog.gradient", 1.5f);

	//Wave
	waveShader.installShaders(wave_vS.c_str(), fS.c_str());
	glUseProgram(waveShader.ID());
	waveShader.GetUniformInfoFromShader();

	clock.start();

	model->Initialize(tex, tex);
	m_wave_object.reset(new eObject);
	m_wave_object->SetModel(model.release());
	m_wave_object->SetTransform(new Transform);
	//@todo move this outside
	m_wave_object->GetTransform()->setTranslation(glm::vec3(3.0f, 2.0f, 0.0f));
	m_wave_object->GetTransform()->setScale(glm::vec3(0.03f, 0.03f, 0.03f));
	m_wave_object->GetTransform()->setRotation(PI / 2, 0.0f, 0.0f);

	LightingIndexDirectionalWave = glGetSubroutineIndex(waveShader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongDirectionalSpecDif");
	LightingIndexPointWave = glGetSubroutineIndex(waveShader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongPointSpecDif");
	LightingIndexSpotWave = glGetSubroutineIndex(waveShader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongFlashSpecDif");

	waveShader.SetUniformData("Fog.maxDist", 40.0f);
	waveShader.SetUniformData("Fog.minDist", 20.0f);
	waveShader.SetUniformData("Fog.color", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	waveShader.SetUniformData("Fog.fog_on", true);
	waveShader.SetUniformData("Fog.density", 0.007f);
	waveShader.SetUniformData("Fog.gradient", 1.5f);
}

//---------------------------------------------------------------------------------
ePhongRender::~ePhongRender()
{
}

//-----------------------------------------------------------------------------------------------------
void ePhongRender::Render(const Camera&								camera,
						             const Light&									light,
						             const std::vector<shObject>&	objects)
{
	glUseProgram(mainShader.ID());
	_SetCommonVariables(camera, light, mainShader, LightingIndexDirectional, LightingIndexPoint, LightingIndexSpot);

	glm ::mat4 worldToProjectionMatrix = camera.getProjectionMatrix() * camera.getWorldToViewMatrix();
	for (auto &object : objects)
	{
		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->GetTransform()->getModelMatrix();
		mainShader.SetUniformData("modelToProjectionMatrix", modelToProjectionMatrix);
		mainShader.SetUniformData("modelToWorldMatrix", object->GetTransform()->getModelMatrix());

		if (object->IsTextureBlending())
			mainShader.SetUniformData("texture_blending", true);
		else
			mainShader.SetUniformData("texture_blending", false);

		if (object->GetRigger() != nullptr)
		{
			matrices = object->GetRigger()->GetMatrices();
		}
		else
		{
			for (auto& m : matrices)
				m = UNIT_MATRIX;
		}
		glUniformMatrix4fv(BonesMatLocation, MAX_BONES, GL_FALSE, &matrices[0][0][0]);
		object->GetModel()->Draw();
	}
}

//---------------------------------------------------------------------------
void ePhongRender::RenderWaves(const Camera& camera, const Light& light, std::vector<shObject> flags)
{
	glUseProgram(waveShader.ID());
	_SetCommonVariables(camera, light, waveShader, LightingIndexDirectionalWave, LightingIndexPointWave, LightingIndexSpotWave);

	waveShader.SetUniformData("Time", time);
	int32_t msc = (int32_t)clock.newFrame();
	float dur = (float)msc / 1000.0f;
	time += dur;

	glDisable(GL_CULL_FACE); //@todo transfer
	glUniform1i(glGetUniformLocation(waveShader.ID(), "normalMapping"), GL_FALSE);

	glm::mat4 worldToProjectionMatrix = camera.getProjectionMatrix() * camera.getWorldToViewMatrix();
	for (auto& flag : flags)
	{
		m_wave_object->GetTransform()->setTranslation(flag->GetTransform()->getTranslation());
		m_wave_object->GetTransform()->setScale(flag->GetTransform()->getScaleAsVector());
		m_wave_object->GetTransform()->billboard(-camera.getDirection());

		glm::quat cur = m_wave_object->GetTransform()->getRotation();
		glm::quat plus = glm::toQuat(glm::rotate(UNIT_MATRIX, (float)PI / 2, XAXIS));
		m_wave_object->GetTransform()->setRotation(cur * plus);

		// set with texture id
		m_wave_object->GetModel()->SetMaterial(flag->GetModel()->GetMaterial().value());

		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * m_wave_object->GetTransform()->getModelMatrix();
		waveShader.SetUniformData("MVP", modelToProjectionMatrix);
		waveShader.SetUniformData("modelToWorldMatrix", m_wave_object->GetTransform()->getModelMatrix());

		glm::mat4 modelViewMatrix = camera.getWorldToViewMatrix() * m_wave_object->GetTransform()->getModelMatrix();
		waveShader.SetUniformData("ModelViewMatrix", modelViewMatrix);
		m_wave_object->GetModel()->Draw();
	}

	glUniform1i(glGetUniformLocation(waveShader.ID(), "normalMapping"), GL_TRUE);
	glEnable(GL_CULL_FACE); //todo transfer
}

//---------------------------------------------------------------------------
void ePhongRender::SetClipPlane(float Height)
{
	glUseProgram(mainShader.ID());
	GLuint clipPlaneLoc = glGetUniformLocation(mainShader.ID(), "clip_plane");
	glUniform4f(clipPlaneLoc, 0, 1, 0, Height);
}

//---------------------------------------------------------------------------
void ePhongRender::_SetCommonVariables(const Camera& camera, const Light& light, Shader& _shader,
																			 GLuint _IndexDirectional, GLuint _IndexPoint, GLuint _IndexSpot)
{
	_shader.SetUniformData("debug_white_color", m_debug_white);
	_shader.SetUniformData("debug_white_texcoords", m_debug_text_coords);
	_shader.SetUniformData("gamma_correction", m_gamma_correction);
	_shader.SetUniformData("tone_mapping", m_tone_mapping);
	_shader.SetUniformData("hdr_exposure", m_exposure);
	_shader.SetUniformData("ssao_threshold", m_ssao_threshold);
	_shader.SetUniformData("ssao_strength", m_ssao_strength);
	_shader.SetUniformData("emission_strength", m_emission_strength);

	_shader.SetUniformData("light.ambient", light.ambient);
	_shader.SetUniformData("light.diffuse", light.diffuse);
	_shader.SetUniformData("light.specular", light.specular);
	_shader.SetUniformData("light.position", light.light_position);
	_shader.SetUniformData("light.direction", light.light_direction);

	_shader.SetUniformData("light.constant", light.constant);
	_shader.SetUniformData("light.linear", light.linear);
	_shader.SetUniformData("light.quadratic", light.quadratic);
	_shader.SetUniformData("light.cutOff", light.cutOff);
	_shader.SetUniformData("light.outerCutOff", light.outerCutOff);

	_shader.SetUniformData("view", camera.getWorldToViewMatrix());

	if (light.type == eLightType::POINT)
	{
		_shader.SetUniformData("shininess", 32.0f);
		glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + glm::vec3(light.light_direction),
			glm::vec3(0.0f, 1.0f, 0.0f));
		_shader.SetUniformData("shadow_directional", false);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_IndexPoint);
		shadowMatrix = camera.getProjectionBiasedMatrix() * worldToViewMatrix;
	}
	else if (light.type == eLightType::SPOT)
	{
		_shader.SetUniformData("shininess", 32.0f);
		glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + glm::vec3(light.light_direction),
			glm::vec3(0.0f, 1.0f, 0.0f));
		_shader.SetUniformData("shadow_directional", true); // ?
		_shader.SetUniformData("use_csm_shadows", false);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_IndexSpot);
		shadowMatrix = camera.getProjectionOrthoMatrix() * worldToViewMatrix;
	}
	else if (light.type == eLightType::DIRECTION)
	{
		_shader.SetUniformData("shininess", 64.0f);
		glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position),
			glm::vec3(0.0f, 0.0f, 0.0f), /*glm::vec3(light.light_position) + light.light_direction,*/
			glm::vec3(0.0f, 1.0f, 0.0f));
		_shader.SetUniformData("shadow_directional", true);
		_shader.SetUniformData("use_csm_shadows", false);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_IndexDirectional);
		shadowMatrix = camera.getProjectionOrthoMatrix() * worldToViewMatrix;
	}
	else if (light.type == eLightType::CSM)
	{
		_shader.SetUniformData("shininess", 64.0f);
		_shader.SetUniformData("shadow_directional", true);
		_shader.SetUniformData("use_csm_shadows", true);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_IndexDirectional);
		_shader.SetUniformData("farPlane", camera.getFarPlane());
		_shader.SetUniformData("cascadeCount", m_shadowCascadeLevels.size());
		for (size_t i = 0; i < m_shadowCascadeLevels.size(); ++i)
		{
			_shader.SetUniformData("cascadePlaneDistances[" + std::to_string(i) + "]", m_shadowCascadeLevels[i]);
		}
	}

	_shader.SetUniformData("shadowMatrix", shadowMatrix); //shadow
	_shader.SetUniformData("eyePositionWorld", glm::vec4(camera.getPosition(), 1.0f));
	_shader.SetUniformData("far_plane", camera.getFarPlane());
	_shader.SetUniformData("farPlane", camera.getFarPlane()); //@todo
}

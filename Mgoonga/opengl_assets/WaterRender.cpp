#include "stdafx.h"
#include <base/Object.h>

#include "WaterRender.h"
#include "MyModel.h"
#include "Texture.h"
#include "GlBufferContext.h"

#include <math/Transform.h>

//--------------------------------------------------------------------------------------------------
eWaterRender::eWaterRender(std::unique_ptr<MyModel> model,
													 const Texture*				waves,
													 const Texture*				DUDV,
													 const std::string&		vertexShaderPath,
													 const std::string&		fragmentShaderPath)
{
	waterShader.installShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
	
	water_model = model.get();

	Material m = model->GetMaterial().value();
	m.normal_texture_id = waves->m_id;
	m.roughness_texture_id = DUDV->m_id;
	model->SetMaterial(m);

	object.reset(new eObject);
	object->SetModel(model.release());
	object->SetTransform(new Transform);

	object->GetTransform()->setTranslation(glm::vec3(0.0f, waterHeight, 0.0f));
	object->GetTransform()->setRotation(PI / 2, 0.0f, 0.0f);
	object->GetTransform()->setScale(glm::vec3(1.25f, 1.8f, 1.0f)); // the size of the pixture

	clock.start();
}

//--------------------------------------------------------------------------------------------------
void eWaterRender::Render(const Camera& _camera, const Light& _light)
{
	glUseProgram(waterShader.ID());
	auto uniforms = waterShader.GetUniforms();

	glm::mat4 worldToProjectionMatrix = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix();

	Texture texture_reflection = eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFLECTION);
	Texture texture_refraction = eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFRACTION);
	
	Material m = water_model->GetMaterial().value();
	m.albedo_texture_id = texture_reflection.m_id;
	m.metalic_texture_id = texture_refraction.m_id;
	water_model->SetMaterial(m);

	waterShader.SetUniformData("moveFactor", move_factor);

	int64_t msc = clock.newFrame();
	move_factor += (float)msc / wave_speed_fator;

	waterShader.SetUniformData("cameraPosition", glm::vec4(_camera.getPosition(), 1.0f));
	waterShader.SetUniformData("lightPosition", _light.light_position);
	waterShader.SetUniformData("lightColor", _light.diffuse);
	
	glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->GetTransform()->getModelMatrix();
	waterShader.SetUniformData("modelToProjectionMatrix", modelToProjectionMatrix);
	waterShader.SetUniformData("modelToWorldMatrix", object->GetTransform()->getModelMatrix());
	
	if(auto it = std::find_if(uniforms.begin(), uniforms.end(), [](const Uniform& _u) { return _u.name == "tiling"; });
		it != uniforms.end() && std::get<float>(it->data) != m_tiling)
			waterShader.SetUniformData("tiling", m_tiling);
	if (auto it = std::find_if(uniforms.begin(), uniforms.end(), [](const Uniform& _u) { return _u.name == "waveStrength"; });
		it != uniforms.end() && std::get<float>(it->data) != m_waveStrength)
			waterShader.SetUniformData("waveStrength", m_waveStrength);
	if (auto it = std::find_if(uniforms.begin(), uniforms.end(), [](const Uniform& _u) { return _u.name == "shineDumper"; });
		it != uniforms.end() && std::get<float>(it->data) != m_shineDumper)
			waterShader.SetUniformData("shineDumper", m_shineDumper);
	if (auto it = std::find_if(uniforms.begin(), uniforms.end(), [](const Uniform& _u) { return _u.name == "reflactivity"; });
		it != uniforms.end() && std::get<float>(it->data) != m_reflactivity)
			waterShader.SetUniformData("reflactivity", m_reflactivity);
	if (auto it = std::find_if(uniforms.begin(), uniforms.end(), [](const Uniform& _u) { return _u.name == "water_color"; });
		it != uniforms.end() && std::get<glm::vec4>(it->data) != m_water_color)
			waterShader.SetUniformData("water_color", m_water_color);
	if (auto it = std::find_if(uniforms.begin(), uniforms.end(), [](const Uniform& _u) { return _u.name == "color_mix"; });
		it != uniforms.end() && std::get<float>(it->data) != m_color_mix)
			waterShader.SetUniformData("color_mix", m_color_mix);
	if (auto it = std::find_if(uniforms.begin(), uniforms.end(), [](const Uniform& _u) { return _u.name == "refrection_factor"; });
		it != uniforms.end() && std::get<float>(it->data) != m_refrection_factor)
			waterShader.SetUniformData("refrection_factor", m_refrection_factor);
	if (auto it = std::find_if(uniforms.begin(), uniforms.end(), [](const Uniform& _u) { return _u.name == "distortion_strength"; });
		it != uniforms.end() && std::get<float>(it->data) != m_distortion_strength)
			waterShader.SetUniformData("distortion_strength", m_distortion_strength);

	object->GetModel()->Draw();
}

//--------------------------------------------------------------------------------------------------
eWaterRender::~eWaterRender()
{
}

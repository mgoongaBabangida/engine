#pragma once

#include "Shader.h"

#include <base/base.h>
#include <math/Camera.h>
#include <math/Timer.h>

#include <memory>

class MyModel;
class eObject;
struct Texture;

//--------------------------------------------------------------------------
class eWaterRender 
{
public:
	eWaterRender(std::unique_ptr<MyModel> model, 
				const Texture* waves,
				const Texture* DUDV,
				const std::string& vertexShaderPath,
				const std::string& fragmentShaderPath);
	~eWaterRender();

	void						Render(const Camera&	camera,
												 const Light&		light);
	
	Shader& GetShader() { return waterShader; }

	float& WaveSpeedFactor() { return wave_speed_fator; }
	float& Tiling() { return m_tiling; }
	float& WaveStrength() { return m_waveStrength; }
	float& ShineDumper() { return m_shineDumper; }
	float& Reflactivity() { return m_reflactivity; }
	glm::vec4& WaterColor() { return m_water_color; }
	float& ColorMix() { return m_color_mix; }
	float& RefrectionFactor() { return m_refrection_factor; }
	float& DistortionStrength() { return m_distortion_strength; }

protected:
	Shader										waterShader;

	std::unique_ptr<eObject>	object;
	MyModel*									water_model = nullptr;
	
	math::eClock		clock;

	float						move_factor	= 0.0f;
	float						waterHeight = 2.0f;
	float						wave_speed_fator = 100'000.0f;

	float			m_tiling =  6.0f;
	float			m_waveStrength = 2.2f;
	float			m_shineDumper = 20.0f;
	float			m_reflactivity = 0.6f;
	glm::vec4 m_water_color = glm::vec4(0.0f, 0.2f, 0.4f, 1.0f);
	float			m_color_mix = 0.2f;
	float			m_refrection_factor = 0.5f;
	float			m_distortion_strength = 0.005f;
};

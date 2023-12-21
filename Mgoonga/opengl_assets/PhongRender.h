#ifndef MAIN_RENDER_H
#define MAIN_RENDER_H

#include "Shader.h"
#include "Texture.h"
#include "TerrainModel.h"
#include <math/Camera.h>
#include <math/Clock.h>
#include <base/Object.h>
#include <base/base.h>

//---------------------------------------------------------------
class ePhongRender
{
public:
	ePhongRender(const std::string& vS, const std::string& wave_vS, const std::string& fS,
		std::unique_ptr<TerrainModel> model, const Texture* tex);
	~ePhongRender();

	void Render(const Camera&			camera,
							const Light&			light,
							const std::vector<shObject>&	objects);

	void RenderWaves(const Camera& camera,
									 const Light& light,
									  std::vector<shObject> objects);

	void	SetClipPlane(float Height);
	void  SetShadowCascadeLevels(const std::vector<float>& _scl) { m_shadowCascadeLevels = _scl; }

	Shader& GetShader() { return mainShader; }

	float& GetSaoThresholdRef() { return m_ssao_threshold; }
	float& GetSaoStrengthRef() { return m_ssao_strength; }

	bool& GetDebugWhite() { return m_debug_white; }
	bool& GetDebugTextCoords() { return m_debug_text_coords; }
	bool& GetGammaCorrection() { return m_gamma_correction; }
	bool& GetToneMapping() { return m_tone_mapping; }
	float& GetExposure() { return m_exposure; }
	float& GetEmissionStrength() { return m_emission_strength; }

protected:
	void _SetCommonVariables(const Camera& camera, const Light& light, Shader&, GLuint _IndexDirectional, GLuint _IndexPoint, GLuint _IndexSpot);

	Shader mainShader;
	Shader waveShader;

	GLuint BonesMatLocation;
	GLuint LightingIndexDirectional;
	GLuint LightingIndexPoint;
	GLuint LightingIndexSpot;

	GLuint LightingIndexDirectionalWave;
	GLuint LightingIndexPointWave;
	GLuint LightingIndexSpotWave;

	float m_ssao_threshold = 0.9f;
	float m_ssao_strength = 0.6f;

	float m_emission_strength = 1.0f;
	bool	m_debug_white = false;
	bool	m_debug_text_coords = false;
	bool	m_gamma_correction = true;
	bool	m_tone_mapping = true;
	float	m_exposure = 1.0f;
	std::vector<float> m_shadowCascadeLevels;

	std::vector<glm::mat4> matrices;
	glm::mat4			         shadowMatrix;

	//wave
	std::unique_ptr<eObject>		m_wave_object;
	math::eClock			clock;
	float							time = 0.0f;
	float							Freq = 2.5f;
	float							Velocity = 2.5f;
	float							Amp = 0.6f;
};

#endif


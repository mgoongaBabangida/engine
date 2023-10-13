#ifndef MAIN_RENDER_H
#define MAIN_RENDER_H

#include "Shader.h"
#include "Texture.h"
#include <math/Camera.h>
#include <base/Object.h>
#include <base/base.h>

//---------------------------------------------------------------
class ePhongRender
{
public:
	ePhongRender(const std::string& vS, const std::string& fS);
	~ePhongRender();

	void Render(const Camera&			camera,
							const Light&			light,
							const std::vector<shObject>&	objects);

	void	SetClipPlane(float Height);
	void	SetShadowMatrix(glm::mat4 shadow_matrix);

	Shader& GetShader() { return mainShader; }

	float& GetSaoThresholdRef() { return m_ssao_threshold; }
	float& GetSaoStrengthRef() { return m_ssao_strength; }

	bool& GetDebugWhite() { return m_debug_white; }
	bool& GetDebugTextCoords() { return m_debug_text_coords; }
	bool& GetGammaCorrection() { return m_gamma_correction; }
	bool& GetToneMapping() { return m_tone_mapping; }
	float& GetExposure() { return m_exposure; }

protected:
	Shader mainShader;

	GLuint LightingIndexDirectional;
	GLuint LightingIndexPoint;
	GLuint LightingIndexSpot;

	GLuint fullTransformationUniformLocation;
	GLuint modelToWorldMatrixUniformLocation;
	GLuint eyePositionWorldUniformLocation;
	GLuint shadowMatrixUniformLocation;
	GLuint BonesMatLocation;

	float m_ssao_threshold = 0.9f;
	float m_ssao_strength = 0.6f;

	bool	m_debug_white = false;
	bool	m_debug_text_coords = false;
	bool	m_gamma_correction = true;
	bool	m_tone_mapping = true;
	float	m_exposure = 1.0f;

	std::vector<glm::mat4> matrices;
	glm::mat4			         shadowMatrix;
};

#endif


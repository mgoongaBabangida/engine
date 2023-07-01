#ifndef MAIN_RENDER_H
#define MAIN_RENDER_H

#include "Shader.h"
#include "Texture.h"
#include <math/Camera.h>
#include <base/Object.h>
#include <base/base.h>

//---------------------------------------------------------------
class eMainRender
{
public:
	eMainRender(const std::string& vS, const std::string& fS);
	~eMainRender();

	void Render(const Camera&			camera,
							const Light&			light,
							const std::vector<shObject>&	objects,
							bool debug_white,
							bool debug_text_coords,
							bool gamma_correction,
							bool tone_mapping,
							float exposure);

	void	SetClipPlane(float Height);
	void	SetShadowMatrix(glm::mat4 shadow_matrix);

	Shader& GetShader() { return mainShader; }
protected:
	Shader mainShader;
	GLuint fullTransformationUniformLocation;
	GLuint modelToWorldMatrixUniformLocation;
	GLuint eyePositionWorldUniformLocation;
	GLuint shadowMatrixUniformLocation;
	GLuint FarPlaneUniformLocation;
	GLuint BonesMatLocation;

	GLuint matAmbientLoc;
	GLuint matDiffuseLoc;
	GLuint matSpecularLoc;
	GLuint matShineLoc;

	GLuint lightAmbientLoc;
	GLuint lightDiffuseLoc;
	GLuint lightSpecularLoc;
	GLuint lightPosLoc;
	GLuint lightDirLoc;

	GLuint LightingIndexDirectional;
	GLuint LightingIndexPoint;
	GLuint LightingIndexSpot;
	
	GLuint lightTypeLoc;

	GLuint DebugWhiteLoc;
	GLuint DebugTexcoordsLoc;

	GLuint GammaCorrectionLoc;
	GLuint ToneMappingLoc;
	GLuint HdrExposureLoc;

	std::vector<glm::mat4> matrices;
	glm::mat4			         shadowMatrix;
};

#endif


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

	void Render(const Camera&			camera,
				      const Light&			light,
				      std::vector<shObject>&	objects,
		          bool debug_white,
		          bool debug_text_coords);

	void	SetClipPlane(float Height);
	void	SetShadowMatrix(glm::mat4 shadow_matrix);

protected:
	Shader mainShader;
	GLuint fullTransformationUniformLocation;
	GLuint modelToWorldMatrixUniformLocation;
	GLuint eyePositionWorldUniformLocation;
	GLuint shadowMatrixUniformLocation;
	GLuint FarPlaneUniformLocation;

	GLuint matAmbientLoc;
	GLuint matDiffuseLoc;
	GLuint matSpecularLoc;
	GLuint matShineLoc;

	GLuint lightAmbientLoc;
	GLuint lightDiffuseLoc;
	GLuint lightSpecularLoc;
	GLuint lightPosLoc;
	GLuint lightDirLoc;

	GLuint LightingIndexPoint;
	GLuint LightingIndexDirectional;
	GLuint lightTypeLoc;

	GLuint DebugWhiteLoc;
	GLuint DebugTexcoordsLoc;

	std::vector<glm::mat4> matrices;
	glm::mat4			         worldToProjectionMatrix;
	glm::mat4			         shadowMatrix;
};

#endif


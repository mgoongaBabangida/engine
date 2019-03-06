#ifndef MAIN_RENDER_H
#define MAIN_RENDER_H

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Structures.h"

class eMainRender
{
public:
	eMainRender();

	void Render(const glm::mat4&		projectionMatrix,
				const Camera&			camera,
				const Light&			light,
				const glm::mat4 &		shadowMatrix,
				std::vector<shObject>&	objects);

	void	SetClipPlane(float Height);
	void	SetShadowMatrix(glm::mat4 shadow_matrix);

protected:
	Shader mainShader;
	GLuint fullTransformationUniformLocation;
	GLuint modelToWorldMatrixUniformLocation;
	GLuint modelViewMatrixLocation;
	GLuint normalMatrixLocation;
	GLuint normalMapingLocation;  //?
	GLuint eyePositionWorldUniformLocation; //
	GLuint shadowMatrixUniformLocation;  //

	GLuint matAmbientLoc;
	GLuint matDiffuseLoc;
	GLuint matSpecularLoc;
	GLuint matShineLoc;

	GLuint lightAmbientLoc;
	GLuint lightDiffuseLoc;
	GLuint lightSpecularLoc;
	GLuint lightPosLoc;
};

#endif

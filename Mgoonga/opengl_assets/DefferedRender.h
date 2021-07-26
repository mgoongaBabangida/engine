#ifndef DEFFERED_RENDER_H
#define DEFFERED_RENDER_H

#include "Shader.h"
#include <math/Camera.h>
#include <base/Object.h>
#include "ScreenMesh.h"

//---------------------------------------------------------------
class eDefferedRender
{
public:
	eDefferedRender(const std::string& vS, const std::string& fS,
					const std::string& vSS, const std::string& fSS);
	void Render(const Camera&			camera,
				std::vector<shObject>&	objects);
	void RenderScreen(const Camera& camera);

protected:
	Shader							          gShader;
	Shader							          screenShader;
	std::unique_ptr<eScreenMesh>	screenMesh;

	GLuint							          fullTransformationUniformLocation;
	GLuint							          modelToWorldMatrixUniformLocation;
	GLuint							          eyePositionWorldUniformLocation;
	GLuint							          viewPosUniformLocation;

	std::vector<glm::vec3>			lightPositions;
	std::vector<glm::vec3>			lightColors;
};

#endif

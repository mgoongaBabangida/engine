#ifndef OUTLINE_RENDER_H
#define OUTLINE_RENDER_H

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Structures.h"

class eOutlineRender
{
public:
	eOutlineRender(const std::string& vS, const std::string& fS);
	void Render(const glm::mat4&		projectionMatrix, 
				const Camera&			camera, 
				const Light&			light, 
				const glm::mat4&		shadowMatrix, 
				std::vector<shObject>&	objects);

private:
	Shader		shader;
	GLuint		fullTransformationUniformLocation;
	GLuint		modelToWorldMatrixUniformLocation;
	GLuint		eyePositionWorldUniformLocation;
};

#endif


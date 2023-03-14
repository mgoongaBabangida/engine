#ifndef SHADOW_RENDER_H
#define SHADOW_RENDER_H

#include "Shader.h"
#include "Texture.h"
#include <math/Camera.h>
#include <base/Object.h>
#include <base/base.h>

//----------------------------------------------------------------------
class eShadowRender 
{
public:
	eShadowRender(const std::string& vS, const std::string& fS, const std::string& gSP, const std::string& fSP);

	void	Render(const Camera&		      camera,
				       const Light&			      light,
				       std::vector<shObject>& objects);

	Shader& GetShader() { return shaderDir; }
protected:
	Shader			shaderDir; //@todo two shaders in one render not good
	Shader			shaderPoint;

	GLuint			MVPUniformLocationDir;
	GLuint			ModelUniformLocationPoint;
	GLuint			ProjectionTransformsUniformLocation;
	GLuint			FarPlaneUniformLocation;

	std::vector<glm::mat4> matrices;
	glm::mat4			         shadowMatrix;
};

#endif
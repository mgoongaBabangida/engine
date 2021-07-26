#ifndef OUTLINE_RENDER_H
#define OUTLINE_RENDER_H

#include "Shader.h"
#include "Texture.h"
#include <math/Camera.h>
#include <base/base.h>
#include <base/Object.h>

//-------------------------------------------------------------------
class eOutlineRender
{
public:
	eOutlineRender(const std::string& vS, const std::string& fS);
	void Render(const Camera&			camera, 
				      const Light&			light, 
				      std::vector<shObject>&	objects);

private:
	Shader		shader;
	GLuint		fullTransformationUniformLocation;
	GLuint		modelToWorldMatrixUniformLocation;
	GLuint		eyePositionWorldUniformLocation;
	
	std::vector<glm::mat4> matrices;
};

#endif


#ifndef SHADOW_RENDER_H
#define SHADOW_RENDER_H

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Structures.h"

class eShadowRender 
{
public:
	eShadowRender(const std::string& vS, const std::string& fS);

	void	Render( const glm::mat4&		projectionMatrix, 
					const Camera&			camera, 
					const Light&			light, 
					std::vector<shObject>&  objects);

protected:
	Shader			shader;
	GLuint			MVPUniformLocation;
};

#endif
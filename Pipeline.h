#ifndef PIPELINE_H
#define PIPELINE_H

#include <glew-2.1.0\include\GL\glew.h>
#include "Structures.h"
#include "RenderManager.h"

class Camera;
class GUI;

class ePipeline
{
public:
	ePipeline(std::vector<shObject>&, uint32_t width, uint32_t height, float nearPlane, float farplane,float waterHeight);
	void			RanderFrame(Camera&, const Light&, std::vector<GUI>&, std::vector<shObject>); //gui should be const latter, and camera prob
	void			Initialize();
	void			InitializeBuffers();
	void			InitializeRenders(eModelManager&, eTextureManager&, const std::string& shadersFolderPath);
	eParticleRender*ParticleRender() { return renderManager.ParticleRender(); } //to improve design

	void			SwitchSkyBox(bool on) { skybox = on; }

protected:
	void			RanderShadows(const Camera&, const Light&, std::vector<shObject>&);
	void			RanderSkybox(const Camera&);
	void			RanderReflection(Camera&, const Light&, std::vector<shObject>&);
	void			RanderRefraction(Camera&, const Light&, std::vector<shObject>&);
	void			RanderSkyNoise(const Camera&);
	void			StencilFuncDefault();
	void			RanderFocused(const Camera&, const Light&, std::vector<shObject>&);
	void			RanderMain(const Camera&, const Light&, std::vector<shObject>&);
	void			RanderOutlineFocused(const Camera&, const Light&, std::vector<shObject>&);
	void			RanderFlags(const Camera&, const Light&);
	void			RanderWater(const Camera&, const Light&);
	void			RanderGeometry(const Camera&);	//hex latter other things
	void			RanderParticles(const Camera&);
	void			RanderBlur(const Camera&);
	void			RanderGui(std::vector<GUI>&, const Camera&);

	bool			mousepress	= false; //to draw framed objects
	bool			mts			= true;
	bool			skybox		= true;
	bool			shadows		= true;
	bool			water		= true;
	bool			sky			= true;
	bool			focuse		= true;
	bool			flags		= true;
	bool			geometry	= true;
	bool			particles	= true;

	glm::mat4		viewToProjectionMatrix;
	glm::mat4		scale_bias_matrix;
	glm::mat4		shadow_matrix;

	uint32_t		width		= 1200;
	uint32_t		height		= 600;
	float			nearPlane	= 0.1f;
	float			farPlane	= 0.0f;
	float			waterHeight = 2.0f;

	std::reference_wrapper<std::vector<shObject>>	m_Objects;
	
	eRenderManager		renderManager;
};

#endif // PIPELINE_H

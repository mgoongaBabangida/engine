#ifndef PIPELINE_H
#define PIPELINE_H

#include <glew-2.1.0\include\GL\glew.h>
#include "opengl_assets.h"

#include <base/base.h>
#include <base/Object.h>

#include "Texture.h"

class Camera;
class GUI;

class eRenderManager;
class eModelManager;
class eTextureManager;

//------------------------------------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS ePipeline
{
public:
	ePipeline(std::vector<shObject>&, uint32_t width, uint32_t height, float nearPlane, float farplane,float waterHeight);
	~ePipeline();

	void			RanderFrame(Camera&, const Light&, std::vector<GUI>&, std::vector<shObject>, std::vector<Flag>&); //gui should be const latter ?, and camera prob
	
	void			Initialize();
	void			InitializeBuffers(bool _needsShadowCubeMap = false);
	void			InitializeRenders(eModelManager&, eTextureManager&, const std::string& shadersFolderPath);

	eRenderManager& GetRenderManager()				{ return *renderManager.get(); } //to delete
	
	void			SetSkyBoxTexture(Texture* _t);
	
	void			AddHex(glm::vec3 _v);
	void			SetHexRadius(float _r);
	
	void			SwitchSkyBox(bool on) { skybox = on; }
	void			SwitchWater(bool on)  { water = on; }

	Texture	  GetSkyNoiseTexture(const Camera& _camera);

  bool& GetBoundingBoxBoolRef() { return draw_bounding_boxes; }
  bool& GetMultiSamplingBoolRef() { return mts; }
  bool& GetSkyBoxOnRef() { return skybox; }
  float& GetBlurCoefRef() { return blur_coef; }

  Texture GetReflectionBufferTexture() const;
  Texture GetRefractionBufferTexture() const;
  Texture GetShadowBufferTexture() const;
  Texture GetGausian1BufferTexture() const;
  Texture GetGausian2BufferTexture() const;
  Texture GetMtsBufferTexture() const;
  Texture GetScreenBufferTexture() const;
  Texture GetBrightFilter() const;

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
	void			RanderFlags(const Camera&, const Light&, std::vector<Flag>&);
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
	bool			draw_bounding_boxes = false;

	uint32_t  width		  = 1200;
	uint32_t  height		= 600;
	float			nearPlane	  = 0.1f;
	float			farPlane	  = 0.0f;
	float			waterHeight = 2.0f;
  float     blur_coef   = 1.0f;

	std::reference_wrapper<std::vector<shObject>>	m_objects;
	
	std::unique_ptr<eRenderManager>			renderManager;
	Texture									            sky_noise_texture;
};

#endif // PIPELINE_H

#ifndef PIPELINE_H
#define PIPELINE_H

#include <glew-2.1.0\include\GL\glew.h>
#include "opengl_assets.h"

#include <base/base.h>
#include <base/Object.h>
#include <math/CameraRay.h>

#include "Texture.h"
#include "RenderManager.h"

#include <map>

class Camera;
class GUI;

class eModelManager;
class eTextureManager;

//------------------------------------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS ePipeline
{
public:

	enum class RenderType
	{ MAIN, PBR, FLAG, OUTLINED};

	ePipeline(dbb::CameraRay&, uint32_t width, uint32_t height);
	~ePipeline();

	void			RenderFrame(std::map<RenderType, std::vector<shObject>>, Camera&, const Light&, std::vector<GUI>&); //gui should be const latter ?, and camera prob
	
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
	float			GetWaterHeight() const { return waterHeight; }

  bool& GetBoundingBoxBoolRef() { return draw_bounding_boxes; }
  bool& GetMultiSamplingBoolRef() { return mts; }
  bool& GetSkyBoxOnRef() { return skybox; }
  float& GetBlurCoefRef() { return blur_coef; }

	float& MaterialMetalness() { return material.metallic; }
	float& MaterialRoughness() { return material.roughness; }
	Material material; //debug

	bool& GetDebugWhite() { return debug_white; }
	bool& GetDebugTexCoords() { return debug_texcoords; }

  Texture GetReflectionBufferTexture() const;
  Texture GetRefractionBufferTexture() const;
  Texture GetShadowBufferTexture() const;
  Texture GetGausian1BufferTexture() const;
  Texture GetGausian2BufferTexture() const;
  Texture GetMtsBufferTexture() const;
  Texture GetScreenBufferTexture() const;
  Texture GetBrightFilter() const;

protected:
	void			RenderShadows(const Camera&, const Light&, std::vector<shObject>&);
	void			RenderSkybox(const Camera&);
	void			RenderReflection(Camera&, const Light&, std::vector<shObject>&);
	void			RenderRefraction(Camera&, const Light&, std::vector<shObject>&);
	void			RenderSkyNoise(const Camera&);
	void			StencilFuncDefault();
	void			RenderFocused(const Camera&, const Light&, const std::vector<shObject>&);
	void			RenderMain(const Camera&, const Light&, const std::vector<shObject>&);
	void			RenderOutlineFocused(const Camera&, const Light&, const std::vector<shObject>&);
	void			RenderFlags(const Camera&, const Light&, std::vector<shObject>);
	void			RenderWater(const Camera&, const Light&);
	void			RenderGeometry(const Camera&);	//hex latter other things
	void			RenderParticles(const Camera&);
	void			RenderBlur(const Camera&);
	void			RenderContrast(const Camera& _camera);
	void			RenderGui(std::vector<GUI>&, const Camera&);
	void			RenderPBR(const Camera&, const Light& _light, std::vector<shObject> _objs);

	bool			mousepress	= true; //to draw framed objects
	bool			mts			= true;
	bool			skybox		= true;
	bool			shadows		= true;
	bool			water		= true;
	bool			sky			= true;
	bool			focuse		= true;
	bool			flags_on		= true;
	bool			geometry	= true;
	bool			particles	= true;
	bool			draw_bounding_boxes = false;
	bool      debug_white = false;
	bool      debug_texcoords = false;

	uint32_t  width		  = 1200;
	uint32_t  height		= 600;
	float			nearPlane	  = 0.1f;
	float			farPlane	  = 0.0f;
	float			waterHeight = 2.0f;
  float     blur_coef   = 0.7f;

	std::reference_wrapper <dbb::CameraRay>				camRay;
	std::unique_ptr<eRenderManager>								renderManager;
};

#endif // PIPELINE_H


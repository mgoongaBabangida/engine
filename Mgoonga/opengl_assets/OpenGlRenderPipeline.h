#ifndef PIPELINE_H
#define PIPELINE_H

#include <glew-2.1.0\include\GL\glew.h>
#include "opengl_assets.h"

#include <base/base.h>
#include <base/Object.h>
#include <math/CameraRay.h>

#include "Texture.h"

#include <map>

class Camera;
class GUI;

class eModelManager;
class eTextureManager;
class eRenderManager;
class SimpleGeometryMesh;

//------------------------------------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS eOpenGlRenderPipeline
{
public:

	enum class RenderType
	{ MAIN, PBR, FLAG, OUTLINED , GEOMETRY};

	eOpenGlRenderPipeline(uint32_t width, uint32_t height);
	~eOpenGlRenderPipeline();

	void			RenderFrame(std::map<RenderType, std::vector<shObject>>, Camera&, const Light&, std::vector<std::shared_ptr<GUI>>&);
	//gui should be const latter ?, and camera prob
	
	void			Initialize();
	void			InitializeBuffers(bool _needsShadowCubeMap = false);
	void			InitializeRenders(eModelManager&, eTextureManager&, const std::string& shadersFolderPath);
	
	const std::vector<ShaderInfo>& GetShaderInfos() const;
	void UpdateShadersInfo();
	void			SetSkyBoxTexture(Texture* _t);
	
	void AddParticleSystem(IParticleSystem* system);
	
	void			SwitchSkyBox(bool on) { skybox = on; }
	void			SwitchWater(bool on)  { water = on; }

	float			GetWaterHeight() const { return waterHeight; }

  bool& GetBoundingBoxBoolRef() { return draw_bounding_boxes; }
  bool& GetMultiSamplingBoolRef() { return mts; }
  bool& GetSkyBoxOnRef() { return skybox; }
	bool& GetWaterOnRef() { return water; }
	bool& GetGeometryOnRef() { return geometry; }
  float& GetBlurCoefRef() { return blur_coef; }
	bool& GetKernelOnRef() { return kernel; }
	bool& GetSkyNoiseOnRef() { return sky_noise; }

	glm::vec4 debug_float = {0.0f,0.0f,0.0f,0.0f};

	bool& GetDebugWhite() { return debug_white; }
	bool& GetDebugTexCoords() { return debug_texcoords; }

	Texture GetDefaultBufferTexture() const;
	Texture GetSkyNoiseTexture(const Camera& _camera);
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
	void			RenderMain(const Camera&, const Light&, const std::vector<shObject>&);
	void			RenderOutlineFocused(const Camera&, const Light&, const std::vector<shObject>&);
	void			RenderFlags(const Camera&, const Light&, std::vector<shObject>);
	void			RenderWater(const Camera&, const Light&);
	void			RenderGeometry(const Camera&, const SimpleGeometryMesh& _mesh);	//hex latter other things
	void			RenderParticles(const Camera&);
	void			RenderBlur(const Camera&);
	void			RenderContrast(const Camera& _camera);
	void			RenderGui(std::vector<std::shared_ptr<GUI>>&, const Camera&);
	void			RenderPBR(const Camera&, const Light& _light, std::vector<shObject> _objs);

	bool			mousepress	= true; //to draw framed objects
	bool			mts			= true;
	bool			skybox		= true;
	bool			shadows		= true;
	bool			water		= true;
	bool			focus		= true;
	bool			flags_on		= true;
	bool			geometry	= true;
	bool			particles	= true;
	bool			draw_bounding_boxes = false;
	bool      debug_white = false;
	bool      debug_texcoords = false;
	bool			kernel = true;
	bool			sky_noise = true;

	const uint32_t  width		  = 1200;
	const uint32_t  height		= 600;

	float			waterHeight = 2.0f;
  float     blur_coef   = 0.5f;

	std::unique_ptr<eRenderManager>	renderManager;
};

#endif // PIPELINE_H


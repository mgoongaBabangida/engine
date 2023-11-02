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
	eOpenGlRenderPipeline(uint32_t width, uint32_t height);
	~eOpenGlRenderPipeline();

	void			RenderFrame(std::map<eObject::RenderType, std::vector<shObject>> _objects,
												std::vector<Camera>& _cameras, //@todo const ?
												const Light& _light,
												std::vector<std::shared_ptr<GUI>>& _guis,
												std::vector<std::shared_ptr<Text>>& _texts);
	//@todo gui should be const latter ?, and camera prob
	
	void			Initialize();
	void			InitializeBuffers();
	void			InitializeRenders(eModelManager&, eTextureManager&, const std::string& shadersFolderPath);
	
	uint32_t  Width() const { return width; }
	uint32_t  Height() const { return height; }

	const std::vector<ShaderInfo>& GetShaderInfos() const;
	void			UpdateShadersInfo();
	bool			SetUniformData(const std::string& _renderName,
													 const std::string& _uniformName,
													 const UniformData& _data);

	void			SetSkyBoxTexture(const Texture* _t);
	void			SetSkyIBL(unsigned int _irr, unsigned int _prefilter);
	
	void AddParticleSystem(std::shared_ptr<IParticleSystem> system);
	void AddParticleSystemGPU(glm::vec3 _startPos, const Texture* _texture);

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
	bool& GetOutlineFocusedRef() { return outline_focused; }
	bool& GetSSAOEnabledRef() { return ssao; }
	bool& GetSSREnabledRef() { return ssr; }
	bool& GetRotateSkyBoxRef();
	float& GetSaoThresholdRef();
	float& GetSaoStrengthRef();
	bool& ShadowingRef() { return shadows; }
	bool& PBBloomRef() { return m_pb_bloom; }

	//Phong Render
	bool& GetDebugWhite();
	bool& GetDebugTexCoords();
	float& GetEmissionStrengthRef();

	//HDR -> LDR blending
	float& GetExposureRef();
	bool& GetGammaCorrectionRef();
	bool& GetToneMappingRef();

	//Water Render
	float& WaveSpeedFactor();
	float& Tiling();
	float& WaveStrength();
	float& ShineDumper();
	float& Reflactivity();
	glm::vec4& WaterColor();
	float& ColorMix();
	float& RefrectionFactor();
	float& DistortionStrength();

	//CSM
	float& ZMult();

	//SSR
	float& Step();
	float& MinRayStep();
	float& MaxSteps();
	int& NumBinarySearchSteps();
	float& ReflectionSpecularFalloffExponent();

	float& Metallic();
	float& Spec();
	glm::vec4& Scale();
	float& K();

	glm::vec4 debug_float = {0.0f,0.0f,0.0f,0.0f};

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
	Texture GetSSAO() const;
	Texture GetDefferedOne() const;
	Texture GetDefferedTwo() const;
	Texture GetHdrCubeMap() const;
	Texture GetLUT() const;
	Texture GetCSMMapLayer1() const;
	Texture GetCSMMapLayer2() const;
	Texture GetCSMMapLayer3() const;
	Texture GetCSMMapLayer4() const;
	Texture GetCSMMapLayer5() const;
	Texture GetBloomTexture() const;
	Texture GetSSRTexture() const;
	Texture GetSSRWithScreenTexture() const;
	Texture GetSSRTextureScreenMask() const;

	void DumpCSMTextures() const;

protected:
	void			RenderShadows(const Camera&, const Light&, std::vector<shObject>&);
	void			RenderShadowsCSM(const Camera& _camera, const Light& _light, std::vector<shObject>& _objects);
	void			RenderSkybox(const Camera&);
	void			RenderReflection(Camera&, const Light&, std::vector<shObject>&, std::vector<shObject>&);
	void			RenderRefraction(Camera&, const Light&, std::vector<shObject>&, std::vector<shObject>&);
	void			RenderSkyNoise(const Camera&);
	void			RenderMain(const Camera&, const Light&, const std::vector<shObject>&);
	void			RenderOutlineFocused(const Camera&, const Light&, const std::vector<shObject>&);
	void			RenderFlags(const Camera&, const Light&, std::vector<shObject>);
	void			RenderWater(const Camera&, const Light&);
	void			RenderGeometry(const Camera&, const SimpleGeometryMesh& _mesh);
	void			RenderParticles(const Camera&);
	void			RenderBlur(const Camera&);
	void			RenderContrast(const Camera& _camera, const Texture& _contrast);
	void			RenderGui(std::vector<std::shared_ptr<GUI>>&, const Camera&);
	void			RenderPBR(const Camera&, const Light& _light, std::vector<shObject> _objs);
	void			RenderSSAO(const Camera&, const Light&, std::vector<shObject>&);
	void			RenderSSR(const Camera& _camera);
	void			RenderIBL(const Camera& _camera);
	void			RenderBloom();

	bool			mts			= true;
	bool			skybox		= true;
	bool			shadows		= true;
	bool			water		= true;
	bool			flags_on		= true;
	bool			geometry	= true;
	bool			particles	= true;
	bool			draw_bounding_boxes = false;
	bool			kernel = false;
	bool			sky_noise = true;
	bool			bezier_curve = true;
	bool			outline_focused = true;
	bool			ssao = false;
	bool			ssr = false;
	bool			m_pb_bloom = false;
	float     blur_coef = 0.5f;

	bool			mousepress = true; //to draw framed objects
	float			waterHeight = 2.0f;
	bool			m_first_call = true;

	const uint32_t  width		  = 1200;
	const uint32_t  height		= 600;

	std::unique_ptr<eRenderManager>	renderManager;
	eTextureManager* m_texture_manager = nullptr;

	Texture csm_dump1;
	Texture csm_dump2;
	Texture csm_dump3;
	Texture csm_dump4;
	Texture csm_dump5;
};

#endif // PIPELINE_H


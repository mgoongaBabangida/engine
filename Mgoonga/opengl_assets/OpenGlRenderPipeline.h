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
	
	void			UpdateSharedUniforms();

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

	std::function<void(const TessellationRenderingInfo&)> GetTessellationInfoUpdater();

	void			SetSkyBoxTexture(const Texture* _t);
	void			SetSkyIBL(unsigned int _irr, unsigned int _prefilter);
	
	void AddParticleSystem(std::shared_ptr<IParticleSystem> system);
	void AddParticleSystemGPU(glm::vec3 _startPos, const Texture* _texture);

	void			SwitchSkyBox(bool on);
	void			SwitchWater(bool on);

	// Getters
	float			GetWaterHeight() const;

  bool& GetBoundingBoxBoolRef() { return draw_bounding_boxes; }
  bool& GetMultiSamplingBoolRef() { return mts; }
  bool& GetSkyBoxOnRef() { return skybox; }
	bool& GetWaterOnRef() { return water; }
	bool& GetGeometryOnRef() { return geometry; }
  float& GetBlurCoefRef() { return blur_coef; }
	bool& GetKernelOnRef();
	bool& GetSkyNoiseOnRef();
	bool& GetOutlineFocusedRef();
	bool& GetSSAOEnabledRef() { return ssao; }
	bool& GetSSREnabledRef() { return ssr; }
	bool& GetEnabledCameraInterpolationRef() { return camera_interpolation; }
	bool& GetRotateSkyBoxRef();
	float& GetSaoThresholdRef();
	float& GetSaoStrengthRef();
	bool& ShadowingRef() { return shadows; }
	bool& PBBloomRef() { return m_pb_bloom; }
	bool& GetMeshLineOn() { return m_mesh_line_on; }
	bool& GetComputeShaderRef() { return m_compute_shader; }

	//Phong Render
	bool& GetDebugWhite();
	bool& GetDebugTexCoords();
	float& GetEmissionStrengthRef();

	//HDR -> LDR blending
	float& GetExposureRef();
	bool& GetGammaCorrectionRef();
	bool& GetToneMappingRef();

	bool& IBL();

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

	// Fog
	struct FogInfo
	{
		float maxDist = 40.0f;
		float minDist = 10.0f;
		glm::vec4 color = glm::vec4 {0.5f,0.5f ,0.5f ,1.0f};
		bool fog_on = true;
		float density = 0.03f;
		float gradient = 4.0f;
	};

	//CameraInterpolation Debug
	glm::vec3& GetSecondCameraPositionRef();
	float& GetDisplacementRef();
	glm::mat4& GetLookAtMatrix();
	glm::mat4& GetProjectionMatrix();
	glm::mat4& GetLookAtProjectedMatrix();

	FogInfo& GetFogInfo() { return m_foginfo; }

	float& Metallic();
	float& Spec();
	glm::vec4& Scale();
	float& K();

	//statistic
	uint32_t& GetDrawCalls() { return m_draw_calls; }

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
	Texture GetCameraInterpolationCoords() const;
	Texture GetComputeParticleSystem() const;

	void DumpCSMTextures() const;

protected:
	void			RenderShadows(const Camera&, const Light&, std::vector<shObject>&);
	void			RenderShadowsCSM(const Camera& _camera, const Light& _light, std::vector<shObject>& _objects);
	void			RenderSkybox(const Camera&);
	void			RenderReflection(Camera&, const Light&, std::vector<shObject>&, std::vector<shObject>&);
	void			RenderRefraction(Camera&, const Light&, std::vector<shObject>&, std::vector<shObject>&);
	void			RenderSkyNoise(const Camera&);
	void			RenderMain(const Camera&, const Light&, const std::vector<shObject>&);
	void			RenderAreaLightsOnly(const Camera& _camera, const Light& _light, const std::vector<shObject>& _objects);
	void			RenderOutlineFocused(const Camera&, const Light&, const std::vector<shObject>&);
	void			RenderFlags(const Camera&, const Light&, std::vector<shObject>);
	void			RenderWater(const Camera&, const Light&);
	void			RenderGeometry(const Camera&, const SimpleGeometryMesh& _mesh);
	void			RenderParticles(const Camera&);
	void			RenderBlur(const Camera&);
	void			RenderContrast(const Camera& _camera, const Texture& _screen, const Texture& _contrast);
	void			RenderGui(std::vector<std::shared_ptr<GUI>>&, const Camera&);
	void			RenderPBR(const Camera&, const Light& _light, std::vector<shObject> _objs);
	void			RenderSSAO(const Camera&, const Light&, std::vector<shObject>&);
	void			RenderSSR(const Camera& _camera);
	void			RenderCameraInterpolationCompute(const Camera& _camera);
	Texture*  RenderCameraInterpolation(const Camera& _camera);
	void			RenderIBL(const Camera& _camera);
	void			RenderBloom();
	void			RenderTerrainTessellated(const Camera&, const Light& _light, std::vector<shObject> _objs);

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
	bool			camera_interpolation = false;
	bool			m_pb_bloom = false;
	float			blur_coef = 0.2f;
	bool			m_mesh_line_on = false;
	bool			ibl_on = true;
	bool			m_compute_shader = false;

	FogInfo		m_foginfo;

	bool			mousepress = true; //to draw framed objects
	float			waterHeight = 2.0f;
	bool			m_first_call = true;

	//statistic
	uint32_t m_draw_calls = 0;

	const uint32_t  width		  = 1200; //@todo make resizable
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


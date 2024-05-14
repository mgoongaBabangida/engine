#pragma once
#include "opengl_assets.h"

#include <base/base.h>

#include "MyModel.h"

struct Texture;
class TerrainMesh;
class Camera;

using TesellationInfoUpdater = std::function<void(const TessellationRenderingInfo&)>;

struct TerrainType
{
	std::string name;
	float       threshold_start;
	float       threshold_finish;
	glm::vec3   color;
};

bool DLL_OPENGL_ASSETS operator<(const TerrainType& _one, const TerrainType& _two);

//---------------------------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS TerrainModel : public ITerrainModel
{
public:
	TerrainModel();
	explicit TerrainModel(Texture* heightMap); //@todo copy generation in private !!!
	
	TerrainModel(Texture* diffuse, Texture* specular, Texture* m_normal, Texture* heightMap);
	TerrainModel(const TerrainModel& other);

	virtual ~TerrainModel();

	void Initialize(const Texture* diffuse, const Texture* specular, const Texture* normal = nullptr, const Texture* heightMap = nullptr,
									bool spreed_texture = true, float _height_scale = 1.0f, float _max_height = 1.0f, float _min_height = 0.0f, int32_t normal_sharpness = 10, unsigned int _tessellation_coef = 16);
	
	void AddOrUpdate(glm::ivec2 _pos, glm::vec2 _offset, const TessellationRenderingInfo&, const Texture* _diffuse, const Texture* heightMap = nullptr,
									 bool spreed_texture = true, int32_t normal_sharpness = 10, bool _apply_normal_blur = false, unsigned int _tessellation_coef = 16);

	void EnableTessellation(bool _enable);
	void SetCamera(Camera* _camera);
	void SetTessellationInfoUpdater(const TesellationInfoUpdater&);
	void SetTessellationInfo(glm::ivec2 _pos, const TessellationRenderingInfo& _info);

	virtual void														Draw()					override;
	virtual const std::string&							GetName() const override;
	virtual const std::string&							GetPath() const override { return m_path; }

  virtual size_t													GetVertexCount() const;
  virtual size_t													GetMeshCount() const { return 1; }
  virtual std::vector<const IMesh*>				GetMeshes() const;
	virtual std::vector<const I3DMesh*>			Get3DMeshes() const;

	virtual bool											HasMaterial() const { return true; }
	virtual void											SetMaterial(const Material& _m);
	virtual std::optional<Material>		GetMaterial() const { return m_material; }

	//ITerrainModel
	float							GetHeight(float x , float z)	override;
	glm::vec3					GetNormal(float x, float z)		override;
	virtual std::vector<std::vector<glm::vec3>>	GetExtremsOfMeshesLocalSpace() const override;

	std::vector<glm::vec3>	GetPositions()			const;
	std::vector<GLuint>		  GetIndeces()			const;
	std::vector<MyMesh*>	  getMeshes()				const;

	void setDiffuse(uint32_t _id);
	void setSpecular(uint32_t _id);
	void setAlbedoTextureArray(const Texture*);
	void setNormalTextureArray(const Texture*);
	void setMetallicTextureArray(const Texture* _t);
	void setRoughnessTextureArray(const Texture* _t);
	void setAOTextureArray(const Texture* _t);

protected:
	std::string									m_path;
	std::vector<TerrainMesh*>		m_meshes;
	Material										m_material;

	Texture											m_height;
	const Texture*							m_albedo_texture_array = nullptr;
	const Texture*							m_normal_texture_array = nullptr;
	const Texture*							m_metallic_texture_array = nullptr;
	const Texture*							m_roughness_texture_array = nullptr;
	const Texture*							m_ao_texture_array = nullptr;
	bool												m_tessellation_enabled = false;
	TesellationInfoUpdater			m_tessellation_info_updater;
	Camera*											m_camera = nullptr;

	//---------------------------------------------------------------------------
	void			_InitMaterialWithDefaults();

	//@todo
	virtual bool														HasBones() const { return false; }
	virtual std::vector<const IBone*>				GetBones() const { return {}; }

	virtual size_t													GetAnimationCount() const { return 0; }
	virtual std::vector<const IAnimation*>	GetAnimations() const {
		return std::vector<const IAnimation*>();
	}
};



#include "stdafx.h"

#include "TerrainModel.h"
#include "TerrainMesh.h"
#include "Texture.h"

#include <math/Camera.h>

#include <algorithm>
#include<cmath> 

//@todo improve constructor - initialization
//----------------------------------------------------------------
TerrainModel::TerrainModel()
{
	_InitMaterialWithDefaults();
}

//----------------------------------------------------------------
TerrainModel::TerrainModel(Texture* diffuse,
													 Texture* specular,
													 Texture* normal,
													 Texture* heightMap)
{
	if (diffuse != nullptr)
		m_material.albedo_texture_id = diffuse->id;

	if (specular != nullptr)
		m_material.metalic_texture_id = specular->id;

	if (normal != nullptr)
		m_material.normal_texture_id = normal->id;

	_InitMaterialWithDefaults();

	m_meshes.push_back(new TerrainMesh("terrain0 0"));
	TerrainMesh* mesh = m_meshes.back();
	mesh->m_size = heightMap->mTextureHeight;
	mesh->m_rows = heightMap->mTextureWidth;
	mesh->m_columns = heightMap->mTextureHeight;
	
	mesh->MakePlaneVerts(heightMap->mTextureWidth,heightMap->mTextureHeight, false);
	mesh->MakePlaneIndices(heightMap->mTextureWidth, heightMap->mTextureHeight);
	mesh->AssignHeights(m_height);
	m_material.normal_texture_id = mesh->GenerateNormals(heightMap->mTextureWidth, heightMap->mTextureHeight)->id;
	mesh->calculatedTangent();
	mesh->setupMesh();
}

//------------------------------------------------------------
TerrainModel::TerrainModel(Texture* color)
{
	if (color != nullptr)
		m_material.albedo_texture_id = color->id;

	if (color != nullptr)
		m_material.metalic_texture_id = color->id;

	_InitMaterialWithDefaults();

	m_meshes.push_back(new TerrainMesh("terrain0 0"));
	TerrainMesh* mesh = m_meshes.back();
	mesh->m_size = 10;
	mesh->MakePlaneVerts(10);
	mesh->MakePlaneIndices(10);
	m_material.normal_texture_id = mesh->GenerateNormals(mesh->m_size)->id;
	mesh->setupMesh();
	mesh->calculatedTangent();
}

//----------------------------------------------------------------
TerrainModel::TerrainModel(const TerrainModel& _other)
  : m_material(_other.m_material)
{
	for (auto& mesh : _other.m_meshes)
		m_meshes.push_back(mesh);
	_InitMaterialWithDefaults();
}

//----------------------------------------------------------------
void TerrainModel::Initialize(const Texture* _diffuse,
														  const Texture* _specular,
														  const Texture* _normal,
														  const Texture* _heightMap,
														  bool spreed_texture,
															float _height_scale,
															float _max_height)
{
	if (_diffuse != nullptr)
		m_material.albedo_texture_id = _diffuse->id;

	if (_specular != nullptr)
		m_material.metalic_texture_id = _specular->id;

	if (_normal != nullptr)
		m_material.normal_texture_id = _normal->id;

	_InitMaterialWithDefaults();

	if (m_meshes.empty())
		m_meshes.push_back(new TerrainMesh("terrain0 0"));

	TerrainMesh* mesh = m_meshes.back();
	if (_heightMap != nullptr)
	{
		m_height = *_heightMap;

		mesh->m_size = _heightMap->mTextureHeight;
		mesh->m_rows = _heightMap->mTextureWidth;
		mesh->m_columns = _heightMap->mTextureHeight;

		mesh->MakePlaneVerts(_heightMap->mTextureWidth, _heightMap->mTextureHeight, spreed_texture);
		//@todo make lod number outside
		mesh->MakePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 1);
		mesh->MakePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 2);
		mesh->MakePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 3);
		mesh->MakePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 4);
		mesh->MakePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 5);
		mesh->AssignHeights(*_heightMap, _height_scale, _max_height);
		m_material.normal_texture_id = mesh->GenerateNormals(_heightMap->mTextureWidth, _heightMap->mTextureHeight)->id;
		mesh->GenerateTessellationData();
	}
	else
	{
		mesh->m_size = _diffuse->mTextureHeight;
		mesh->MakePlaneVerts(_diffuse->mTextureHeight, _diffuse->mTextureHeight, spreed_texture);
		mesh->MakePlaneIndices(_diffuse->mTextureHeight);
		m_material.normal_texture_id = mesh->GenerateNormals(mesh->m_size)->id;
	}

	mesh->calculatedTangent();
	mesh->setupMesh();
	if (m_camera)
		mesh->SetCamera(m_camera);
}

//----------------------------------------------------------------
void TerrainModel::AddOrUpdate(glm::ivec2 _pos, glm::vec2 _offset, const Texture* _diffuse, const Texture* _heightMap, bool spreed_texture, float _height_scale, float _max_height)
{
	TerrainMesh* mesh = nullptr;
	for (auto* m : m_meshes)
	{
		if (m->GetPosition() == _pos)
		{
			mesh = m;
			break;
		}
	}
	if (mesh == nullptr)
	{
		m_meshes.push_back(new TerrainMesh("terrain" + std::to_string(_pos.x) + " " + std::to_string(_pos.y)));
		mesh = m_meshes.back();
	}
	mesh->SetPosition(_pos);
	mesh->SetWorldOffset(_offset);
	if (_heightMap != nullptr)
	{
		m_height = *_heightMap;

		mesh->m_size = _heightMap->mTextureHeight;
		mesh->m_rows = _heightMap->mTextureWidth;
		mesh->m_columns = _heightMap->mTextureHeight;

		mesh->MakePlaneVerts(_heightMap->mTextureWidth, _heightMap->mTextureHeight, spreed_texture);
		//@todo make lod number outside
		mesh->MakePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 1);
		mesh->MakePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 2);
		mesh->MakePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 3);
		mesh->MakePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 4);
		mesh->MakePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 5);
		mesh->AssignHeights(*_heightMap, _height_scale, _max_height);
		m_material.normal_texture_id = mesh->GenerateNormals(_heightMap->mTextureWidth, _heightMap->mTextureHeight)->id;
		mesh->GenerateTessellationData();
	}
	else
	{
		mesh->m_size = _diffuse->mTextureHeight;
		mesh->MakePlaneVerts(_diffuse->mTextureHeight, _diffuse->mTextureHeight, spreed_texture);
		mesh->MakePlaneIndices(_diffuse->mTextureHeight);
		m_material.normal_texture_id = mesh->GenerateNormals(mesh->m_size)->id;
	}

	mesh->calculatedTangent();
	mesh->setupMesh();
	if (m_camera)
		mesh->SetCamera(m_camera);
}

//----------------------------------------------------------------
void TerrainModel::EnableTessellation(bool _enable)
{
	m_tessellation_enabled = _enable;
}

//----------------------------------------------------------------
void TerrainModel::SetCamera(Camera* _camera)
{
	m_camera = _camera;
	for (auto* mesh : m_meshes)
		mesh->SetCamera(_camera);
}

//----------------------------------------------------------------
void TerrainModel::Draw()
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_material.albedo_texture_id);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_material.metalic_texture_id);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_material.normal_texture_id);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_material.roughness_texture_id);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, m_material.emissive_texture_id);

	if (m_albedo_texture_array != nullptr)
	{
		glActiveTexture(GL_TEXTURE12);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_albedo_texture_array->id);

		glBindTextureUnit(12, m_albedo_texture_array->id);
	}

	std::vector<TerrainMesh*> rendered_meshes;
	if (m_camera)
	 {
		for (auto* mesh : m_meshes)
		{
			if(m_camera->getCameraRay().IsInFrustum(mesh->GetExtrems()))
				rendered_meshes.push_back(mesh);
		}
	}
	else
		rendered_meshes = m_meshes;

	for (auto* mesh : rendered_meshes)
	{
		if (!m_tessellation_enabled)
				mesh->Draw();
		else
				mesh->DrawTessellated();
	}
}

//----------------------------------------------------------------
std::vector<MyMesh*>	TerrainModel::getMeshes()	const
{ 
	std::vector<MyMesh*> meshes;
	for (auto& mesh : m_meshes)
		meshes.push_back(mesh);
  return meshes;
}

//------------------------------------------------------------
void	TerrainModel::setDiffuse(uint32_t _id)
{
	m_material.albedo_texture_id = _id;
}

//------------------------------------------------------------
void	TerrainModel::setSpecular(uint32_t _id)
{
	m_material.metallic = _id;
}

//----------------------------------------------------------------
void TerrainModel::setAlbedoTextureArray(const Texture* _t)
{
	m_albedo_texture_array = _t;
}


 //----------------------------------------------------------------
float TerrainModel::GetHeight(float x, float z)
{
	for (auto* mesh : m_meshes)
	{
		std::optional<Vertex> vert = mesh->FindVertex(x, z);
		if (vert != std::nullopt)
			return vert->Position.y;
	}
	return -1.f;
}

//----------------------------------------------------------------
glm::vec3 TerrainModel::GetNormal(float x, float z)
{
	for (auto* mesh : m_meshes)
	{
		std::optional<Vertex> vert = mesh->FindVertex(x, z);
		if (vert != std::nullopt)
			return vert->Normal;
	}
	return glm::vec3();
}

//----------------------------------------------------------------
std::vector<std::vector<glm::vec3>> TerrainModel::GetExtremsOfMeshesLocalSpace() const
{
	std::vector<std::vector<glm::vec3>> ret;
	for (auto& mesh : m_meshes)
		ret.push_back(mesh->GetExtrems());
	return ret;
}

//----------------------------------------------------------------
const std::string& TerrainModel::GetName() const
{
	return m_path;
}

//----------------------------------------------------------------
size_t TerrainModel::GetVertexCount() const
{
	size_t vertexCount = 0;
	if (!m_meshes.empty())
	{
		for (auto* mesh : m_meshes)
			vertexCount += mesh->vertices.size();
	}
  return vertexCount;
}

//----------------------------------------------------------------
std::vector<const IMesh*> TerrainModel::GetMeshes() const
{
	if (!m_meshes.empty())
	{
		std::vector<const IMesh*> meshes;
		for (auto* mesh : m_meshes)
			meshes.push_back(mesh);
		return meshes;
	}
	else
		return {};
}

//----------------------------------------------------------------
std::vector<const I3DMesh*> TerrainModel::Get3DMeshes() const
{
	if (!m_meshes.empty())
	{
		std::vector<const I3DMesh*> meshes;
		for (auto* mesh : m_meshes)
			meshes.push_back(mesh);
		return meshes;
	}
	else
		return {};
}

//----------------------------------------------------------------
void TerrainModel::SetMaterial(const Material& _m)
{
	m_material = _m;
	_InitMaterialWithDefaults();
}

//----------------------------------------------------------------
void TerrainModel::_InitMaterialWithDefaults()
{
	if (m_material.albedo_texture_id == -1)
		m_material.albedo_texture_id = Texture::GetTexture1x1(GREY).id;

	if (m_material.metalic_texture_id == -1)
		m_material.metalic_texture_id = Texture::GetTexture1x1(BLACK).id;

	if (m_material.normal_texture_id == -1)
		m_material.normal_texture_id = Texture::GetTexture1x1(BLUE).id;

	if (m_material.roughness_texture_id == -1)
		m_material.roughness_texture_id = Texture::GetTexture1x1(BLACK).id;

	if (m_material.emissive_texture_id == -1)
		m_material.emissive_texture_id = Texture::GetTexture1x1(BLACK).id;
}

//----------------------------------------------------------------
std::vector<glm::vec3> TerrainModel::GetPositions() const
{
	std::vector<glm::vec3> ret;
	if (m_meshes.empty())
		return ret;

	for (auto& vert : m_meshes[0]->vertices)
		ret.push_back(vert.Position);
	return ret; // @todo to improve
}

//----------------------------------------------------------------
std::vector<GLuint> TerrainModel::GetIndeces() const
{
	return m_meshes[0]->indicesLods[0];
}

//----------------------------------------------------------------
TerrainModel::~TerrainModel()
{
	for (auto* mesh : m_meshes)
		delete mesh;
	//custom normals
 /* normal.freeTexture(); !!!*/
}

//----------------------------------------------------------------
bool operator<(const TerrainType& _one, const TerrainType& _two)
{
	return _one.threshold_start < _two.threshold_start;
}



#include "stdafx.h"

#include "TerrainModel.h"
#include "Texture.h"

#include <algorithm>
#include<cmath> 

//@todo improve constructor - initialization
//----------------------------------------------------------------
TerrainModel::TerrainModel()
	: mesh(nullptr)
{
	m_material.roughness_texture_id = m_material.emissive_texture_id = Texture::GetTexture1x1(BLACK).id;
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

	m_material.roughness_texture_id = m_material.emissive_texture_id = Texture::GetTexture1x1(BLACK).id;

	mesh = new MyMesh("terrain");
	m_size = heightMap->mTextureHeight;
	m_rows = heightMap->mTextureWidth;
	m_columns = heightMap->mTextureHeight;
	
	makePlaneVerts(heightMap->mTextureWidth,heightMap->mTextureHeight, false);
	makePlaneIndices(heightMap->mTextureWidth, heightMap->mTextureHeight);
	assignHeights(m_height);
	generateNormals(heightMap->mTextureWidth, heightMap->mTextureHeight);
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

	m_material.roughness_texture_id = m_material.emissive_texture_id = Texture::GetTexture1x1(BLACK).id;

	mesh = new MyMesh("terrain");
	m_size = 10;
	makePlaneVerts(10);
	makePlaneIndices(10);
	generateNormals(m_size);
	mesh->setupMesh();
	mesh->calculatedTangent();
}

//----------------------------------------------------------------
TerrainModel::TerrainModel(const TerrainModel& _other)
  : mesh(_other.mesh)
  , m_material(_other.m_material)
{}

//----------------------------------------------------------------
void TerrainModel::initialize(const Texture* _diffuse,
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

	if (mesh == nullptr)
		mesh = new MyMesh("terrain");

	if (_heightMap != nullptr)
	{
		m_height = *_heightMap;

		m_size = _heightMap->mTextureHeight;
		m_rows = _heightMap->mTextureWidth;
		m_columns = _heightMap->mTextureHeight;

		makePlaneVerts(_heightMap->mTextureWidth, _heightMap->mTextureHeight, spreed_texture);
		//@todo make lod number outside
		makePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 1);
		makePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 2);
		makePlaneIndices(_heightMap->mTextureWidth, _heightMap->mTextureHeight, 3);
		assignHeights(*_heightMap, _height_scale, _max_height);
		generateNormals(_heightMap->mTextureWidth, _heightMap->mTextureHeight);
	}
	else
	{
		m_size = _diffuse->mTextureHeight;
		makePlaneVerts(_diffuse->mTextureHeight, _diffuse->mTextureHeight, spreed_texture);
		makePlaneIndices(_diffuse->mTextureHeight);
		generateNormals(m_size);
	}

	mesh->calculatedTangent();
	mesh->setupMesh();
	//does my mesh need textures ?
  //mesh->setTextures({ &diffuse , &specular , &normal , &m_height });
}

//----------------------------------------------------------------
std::vector<MyMesh*>	TerrainModel::getMeshes()	const
{ 
  return std::vector<MyMesh*>{mesh};
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
 Vertex TerrainModel::findVertex(float x, float z)
{
	float newX = glm::floor(x);
	newX = newX + (glm::round((x - newX) * devisor)) / devisor;
	float newZ = glm::floor(z);
	newZ = newZ + (glm::round((z - newZ) * devisor)) / devisor;

	auto vert = std::find_if(mesh->vertices.begin(), mesh->vertices.end(), [newX, newZ](const Vertex& v)
													{return v.Position.x == newX && v.Position.z == newZ; });
	if (vert != mesh->vertices.end())
		return *vert;
	else
		return Vertex();// std::optional
}

 //----------------------------------------------------------------
float TerrainModel::GetHeight(float x, float z)
{
	Vertex vert = findVertex(x, z);
		return vert.Position.y;
}

//----------------------------------------------------------------
glm::vec3 TerrainModel::GetNormal(float x, float z)
{
	Vertex vert = findVertex(x, z);
	return vert.Normal;
}

//----------------------------------------------------------------
void TerrainModel::assignHeights(const Texture& _heightMap, float _height_scale, float _max_height)
{
	glBindTexture(GL_TEXTURE_2D, _heightMap.id);
	if (_heightMap.mChannels == 4)
	{
		GLfloat* buffer = new GLfloat[_heightMap.mTextureHeight * _heightMap.mTextureWidth * 4];

		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, buffer);
		int counter = 0;
		for (int i = 0; i < _heightMap.mTextureHeight * _heightMap.mTextureWidth * 4; i += 4)
		{
			float height = (float)(buffer[i] * _height_scale);
			mesh->vertices[i / 4].Position.y = height <= _max_height ? height : _max_height;
		}
		delete[] buffer;
	}
	else if (_heightMap.mChannels == 1)
	{
		GLfloat* buffer = new GLfloat[_heightMap.mTextureHeight * _heightMap.mTextureWidth];

		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, buffer);
		int counter = 0;
		for (int i = 0; i < _heightMap.mTextureHeight * _heightMap.mTextureWidth; ++i)
		{
			float height = (float)(buffer[i] * _height_scale);
			mesh->vertices[i].Position.y = height <= _max_height ? height : _max_height;
		}
		delete[] buffer;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

//----------------------------------------------------------------
void TerrainModel::generateNormals(GLuint size)
{
	for (unsigned int i = 0; i < mesh->indicesLods[0].size(); i += 3) 
	{
		glm::vec3& pos1 = mesh->vertices[mesh->indicesLods[0][i]].Position;
		glm::vec3& pos2 = mesh->vertices[mesh->indicesLods[0][i + 1]].Position;
		glm::vec3& pos3 = mesh->vertices[mesh->indicesLods[0][i + 2]].Position;
		glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(pos3 - pos1), glm::vec3(pos2 - pos1)));  //side ?
		mesh->vertices[mesh->indicesLods[0][i]].Normal += normal;
		mesh->vertices[mesh->indicesLods[0][i + 1]].Normal += normal;
		mesh->vertices[mesh->indicesLods[0][i + 2]].Normal += normal;
	}
	for (unsigned int i = 0; i < mesh->vertices.size(); ++i)
	{
		mesh->vertices[i].Normal = glm::normalize(mesh->vertices[i].Normal);
	}

	// Load to normal map
	if (false)
	{
		GLfloat* buffer = new GLfloat[mesh->vertices.size() * 4];
		for (int i = 0; i < mesh->vertices.size(); ++i) {
			buffer[i * 4] = mesh->vertices[i].Normal.x;//(mesh->vertices[i].Normal.x + 1.0f) / 2.0f;  // ( - 0.5) *2;//(GLubyte)128.f;
			buffer[i * 4 + 1] = mesh->vertices[i].Normal.z; //(mesh->vertices[i].Normal.y+1.0f)/2.0f;128
			buffer[i * 4 + 2] = mesh->vertices[i].Normal.y;// (mesh->vertices[i].Normal.z + 1.0f) / 2.0f; //(mesh->vertices[i].Normal.z+1.0f)/2.0f;255
			buffer[i * 4 + 3] = 1.0f;
		}

		Texture normal = Texture(size, size, 4); // free texture !!!
		normal.TextureFromBuffer(buffer, size, size);
		m_material.normal_texture_id = normal.id;
		delete[] buffer;
	}
}

//----------------------------------------------------------------
void TerrainModel::generateNormals(GLuint rows, GLuint columns)
{
	for (unsigned int i = 0; i < mesh->indicesLods[0].size(); i += 3) 
	{
		glm::vec3& pos1 = mesh->vertices[mesh->indicesLods[0][i]].Position;
		glm::vec3& pos2 = mesh->vertices[mesh->indicesLods[0][i + 1]].Position;
		glm::vec3& pos3 = mesh->vertices[mesh->indicesLods[0][i + 2]].Position;
		glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(pos3 - pos1), glm::vec3(pos2 - pos1)));  //side ?
		
		if (normal.y < 0)
			normal = -normal;
		mesh->vertices[mesh->indicesLods[0][i]].Normal += normal;
		mesh->vertices[mesh->indicesLods[0][i + 1]].Normal += normal;
		mesh->vertices[mesh->indicesLods[0][i + 2]].Normal += normal;
	}

	for (unsigned int i = 0; i < mesh->vertices.size(); ++i)
	{
		mesh->vertices[i].Normal = glm::normalize(mesh->vertices[i].Normal);
	}

	if (false) 		// Load to normal map
	{
		GLfloat* buffer = new GLfloat[mesh->vertices.size() * 4];
		for (int i = 0; i < mesh->vertices.size(); ++i)
		{
			buffer[i * 4]			= (mesh->vertices[i].Normal.x + 1.0f) / 2.0f;
			buffer[i * 4 + 1] = (mesh->vertices[i].Normal.z + 1.0f) / 2.0f;
			buffer[i * 4 + 2] = (mesh->vertices[i].Normal.y + 1.0f) / 2.0f;
			buffer[i * 4 + 3] = 1.0f;
		}

		Texture normal = Texture(rows, columns, 4); // free texture !!!
		normal.TextureFromBuffer(buffer, rows, columns);
		m_material.normal_texture_id = normal.id;
		delete[] buffer;
	}
}

//----------------------------------------------------------------
void TerrainModel::makePlaneVerts(unsigned int dimensions, bool spreed_texture)
{
	mesh->vertices.resize(dimensions * dimensions);
	int half = dimensions / 2;
	for (int i = 0; i < dimensions; i++)
	{
		for (int j = 0; j < dimensions; j++)
		{
			Vertex& thisVert = mesh->vertices[i * dimensions + j];
			thisVert.Position.x =(float) (j - half) / devisor;
			thisVert.Position.z = (float)(i - half) / devisor;
			thisVert.Position.y = 0;
			thisVert.Normal = glm::vec3(0.0f, 1.0f, 0.0f);

      if (spreed_texture)
      {
        thisVert.TexCoords.x = j / (float)dimensions;
        thisVert.TexCoords.y = i / (float)dimensions;
      }
      else
      {
        thisVert.TexCoords.x = j % 2 ? 0.0f : 1.0f;
        thisVert.TexCoords.y = i % 2 ? 0.0f : 1.0f;
      }
		}
	}
}

//----------------------------------------------------------------
void TerrainModel::makePlaneVerts(unsigned int rows, unsigned int columns, bool spreed_texture)
{
	mesh->vertices.resize(rows * columns);
	int half_r = rows / 2;
	int half_c = columns / 2;
	int counter = 0;
	float MinX = 0,  MinZ = 0, MaxX = 0, MaxZ = 0; //debug only
	for (int i = 0; i < columns; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			Vertex& thisVert = mesh->vertices[i * rows + j];
			thisVert.Position.x = static_cast<float>(j - half_r) / static_cast<float>(devisor);
			thisVert.Position.z = static_cast<float>(i - half_c) / static_cast<float>(devisor);
			thisVert.Position.y = 0;
			thisVert.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
      if (spreed_texture)
      {
        thisVert.TexCoords.x = static_cast<float>(j) / static_cast<float>(rows);
        thisVert.TexCoords.y = static_cast<float>(i) / static_cast<float>(columns);
      }
      else
      {
        thisVert.TexCoords.x = j % 2 ? 0.0f : 1.0f;
        thisVert.TexCoords.y = i % 2 ? 1.0f : 0.0f;
      }
			if (thisVert.Position.x < MinX)  //debug only
				MinX = thisVert.Position.x;
			if (thisVert.Position.x > MaxX)
				MaxX = thisVert.Position.x;
			if (thisVert.Position.z < MinZ)
				MinZ = thisVert.Position.z;
			if (thisVert.Position.z > MaxZ)
				MaxZ = thisVert.Position.z;
			counter++;
		}
	}
}

//----------------------------------------------------------------
void TerrainModel::makePlaneIndices(unsigned int dimensions)
{
	mesh->indicesLods[0].resize((dimensions) * (dimensions) * 2 * 3);// 2 triangles per square, 3 indices per triangle dim-1???
		int runner = 0;
	for (int row = 0; row < dimensions - 1; row++)
	{
		for (int col = 0; col < dimensions - 1; col++) //order?
		{
			mesh->indicesLods[0][runner++] = dimensions * row + col;
			mesh->indicesLods[0][runner++] = dimensions * row + col + dimensions;
			mesh->indicesLods[0][runner++] = dimensions * row + col + dimensions + 1;

			mesh->indicesLods[0][runner++] = dimensions * row + col;
			mesh->indicesLods[0][runner++] = dimensions * row + col + dimensions + 1;
			mesh->indicesLods[0][runner++] = dimensions * row + col + 1;
		}
	}
}

//----------------------------------------------------------------
void TerrainModel::makePlaneIndices(unsigned int rows,unsigned int columns, unsigned int _lod)
{
	if (mesh->indicesLods.size() < _lod)
		mesh->indicesLods.push_back({});

	mesh->indicesLods[_lod-1].resize((rows) * (columns) * 2 * 3);// 2 triangles per square, 3 indices per triangle dim-1???
	int runner = 0;
	float MinX = 0, MinZ = 0, MaxX = 0, MaxZ = 0;
	unsigned int lod_scale = pow(2,(_lod-1));
	for (int col = 0; col < columns- lod_scale; col+= lod_scale)
	{
		for (int row = 0; row < rows- lod_scale; row+= lod_scale)
		{
			mesh->indicesLods[_lod-1][runner++] = col * rows + row;
			mesh->indicesLods[_lod-1][runner++] = col * rows + row + rows* lod_scale;
			mesh->indicesLods[_lod-1][runner++] = col * rows + row + rows* lod_scale + lod_scale;
			
			mesh->indicesLods[_lod-1][runner++] = col * rows + row;
			mesh->indicesLods[_lod-1][runner++] = col * rows + row + rows* lod_scale + lod_scale;
			mesh->indicesLods[_lod-1][runner++] = col * rows + row + lod_scale;
			
			for (int i = 0; i < 6 && runner < mesh->indicesLods[_lod - 1].size(); ++i)
			{
				if (mesh->vertices[mesh->indicesLods[_lod - 1][runner - i]].Position.x < MinX)
					MinX = mesh->vertices[mesh->indicesLods[_lod - 1][runner - i]].Position.x;
				if (mesh->vertices[mesh->indicesLods[_lod - 1][runner - i]].Position.x > MaxX)
					MaxX = mesh->vertices[mesh->indicesLods[_lod - 1][runner - i]].Position.x;
				if (mesh->vertices[mesh->indicesLods[_lod - 1][runner - i]].Position.z < MinZ)
					MinZ = mesh->vertices[mesh->indicesLods[_lod - 1][runner - i]].Position.z;
				if (mesh->vertices[mesh->indicesLods[_lod - 1][runner - i]].Position.z > MaxZ)
					MaxZ = mesh->vertices[mesh->indicesLods[_lod - 1][runner - i]].Position.z;
			}
		}
	}
	mesh->indicesLods[_lod - 1].resize(runner);
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

	mesh->Draw();
}

//----------------------------------------------------------------
size_t TerrainModel::GetVertexCount() const
{
  return mesh->vertices.size();
}

//----------------------------------------------------------------
std::vector<const IMesh*> TerrainModel::GetMeshes() const
{
  return std::vector<const IMesh*>{mesh};
}

//----------------------------------------------------------------
std::vector<const I3DMesh*> TerrainModel::Get3DMeshes() const
{
	return std::vector<const I3DMesh*>{mesh};
}

//----------------------------------------------------------------
std::vector<glm::vec3> TerrainModel::GetPositions() const
{
	std::vector<glm::vec3> ret;
	for (auto& vert : mesh->vertices)
		ret.push_back(vert.Position);
	return ret; // @todo to improve
}

//----------------------------------------------------------------
std::vector<GLuint> TerrainModel::GetIndeces() const
{
	return mesh->indicesLods[0];
}

//----------------------------------------------------------------
TerrainModel::~TerrainModel()
{
	if (mesh != nullptr)
		delete mesh;
	//custom normals
 /* normal.freeTexture(); !!!*/
}

//----------------------------------------------------------------
bool operator<(const TerrainType& _one, const TerrainType& _two)
{
	return _one.threshold_start < _two.threshold_start;
}



#include "stdafx.h"

#include "TerrainModel.h"
#include "Texture.h"

#include <algorithm>

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
void TerrainModel::initialize(const Texture* diffuse, const Texture* specular, bool spreed_texture)
{
	if (diffuse != nullptr)
		m_material.albedo_texture_id = diffuse->id;

	if (specular != nullptr)
		m_material.metalic_texture_id = specular->id;

  mesh = new MyMesh("terrain");
	m_size = diffuse->mTextureHeight;
	makePlaneVerts(diffuse->mTextureHeight, diffuse->mTextureHeight, spreed_texture);
	makePlaneIndices(diffuse->mTextureHeight);
	generateNormals(m_size);
	
	mesh->calculatedTangent();
	mesh->setupMesh();
	//does my mesh need textures ?
  //mesh->setTextures({ diffuse , specular , &m_normal , &m_height });
}

//----------------------------------------------------------------
void TerrainModel::initialize(const Texture* diffuse,
														  const Texture* specular,
														  const Texture* normal,
														  const Texture* heightMap,
														  bool spreed_texture)
{
	if (diffuse != nullptr)
		m_material.albedo_texture_id = diffuse->id;

	if (specular != nullptr)
		m_material.metalic_texture_id = specular->id;

	if (normal != nullptr)
		m_material.normal_texture_id = normal->id;

	m_height =	*heightMap;
	
	if(mesh == nullptr)
	  mesh = new MyMesh("terrain");

	m_size = heightMap->mTextureHeight;
	m_rows = heightMap->mTextureWidth;
	m_columns = heightMap->mTextureHeight;

	makePlaneVerts(heightMap->mTextureWidth, heightMap->mTextureHeight, spreed_texture);
	makePlaneIndices(heightMap->mTextureWidth, heightMap->mTextureHeight);
	assignHeights(*heightMap);
	generateNormals(heightMap->mTextureWidth, heightMap->mTextureHeight);
	
	mesh->calculatedTangent();
	mesh->setupMesh();
	//does my mesh need textures ?
  //mesh->setTextures({ &diffuse , &specular , &normal , &m_height });
}

//----------------------------------------------------------------
std::vector<MyMesh*>	TerrainModel::getMeshes()				const 
{ 
  return std::vector<MyMesh*>{ mesh};
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

float TerrainModel::GetHeight(float x, float z)
{
	Vertex vert = findVertex(x, z);
		return vert.Position.y;
}

glm::vec3 TerrainModel::GetNormal(float x, float z)
{
	Vertex vert = findVertex(x, z);
	return vert.Normal;
}

void TerrainModel::assignHeights(Texture heightMap)
{
	GLfloat* buffer = new GLfloat[heightMap.mTextureHeight * heightMap.mTextureWidth * 4 ]; 

	glBindTexture(GL_TEXTURE_2D, heightMap.id);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);
	int counter = 0;
	for (int i = 0; i < heightMap.mTextureHeight * heightMap.mTextureWidth * 4; i += 4) 
	{
		mesh->vertices[i / 4].Position.y = (float)buffer[i];
		counter++;
	}
	delete[] buffer;
}

void TerrainModel::generateNormals(GLuint size)
{
	for (unsigned int i = 0; i < mesh->indices.size(); i += 3) 
	{
		glm::vec3& pos1 = mesh->vertices[mesh->indices[i]].Position;
		glm::vec3& pos2 = mesh->vertices[mesh->indices[i + 1]].Position;
		glm::vec3& pos3 = mesh->vertices[mesh->indices[i + 2]].Position;
		glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(pos3 - pos1), glm::vec3(pos2 - pos1)));  //side ?
		mesh->vertices[mesh->indices[i]].Normal += normal;
		mesh->vertices[mesh->indices[i + 1]].Normal += normal;
		mesh->vertices[mesh->indices[i + 2]].Normal += normal;
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

		Texture normal = Texture(size, size); // free texture !!!
		normal.TextureFromBuffer(buffer, size, size);
		m_material.normal_texture_id = normal.id;
		delete[] buffer;
	}
}

void TerrainModel::generateNormals(GLuint rows, GLuint columns)
{
	for (unsigned int i = 0; i < mesh->indices.size(); i += 3) 
	{
		glm::vec3& pos1 = mesh->vertices[mesh->indices[i]].Position;
		glm::vec3& pos2 = mesh->vertices[mesh->indices[i + 1]].Position;
		glm::vec3& pos3 = mesh->vertices[mesh->indices[i + 2]].Position;
		glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(pos3 - pos1), glm::vec3(pos2 - pos1)));  //side ?
		
		if (normal.y < 0)
			normal = -normal;
		mesh->vertices[mesh->indices[i]].Normal += normal;
		mesh->vertices[mesh->indices[i + 1]].Normal += normal;
		mesh->vertices[mesh->indices[i + 2]].Normal += normal;
	}

	for (unsigned int i = 0; i < mesh->vertices.size(); ++i)
	{
		mesh->vertices[i].Normal = glm::normalize(mesh->vertices[i].Normal);
	}
	if (false)
	{
		// Load to normal map
		GLfloat* buffer = new GLfloat[mesh->vertices.size() * 4];
		for (int i = 0; i < mesh->vertices.size(); ++i)
		{
			buffer[i * 4]			= (mesh->vertices[i].Normal.x + 1.0f) / 2.0f;
			buffer[i * 4 + 1] = (mesh->vertices[i].Normal.z + 1.0f) / 2.0f;
			buffer[i * 4 + 2] = (mesh->vertices[i].Normal.y + 1.0f) / 2.0f;
			buffer[i * 4 + 3] = 1.0f;
		}

		Texture normal = Texture(rows, columns);// free texture !!!
		normal.TextureFromBuffer(buffer, rows, columns);
		m_material.normal_texture_id = normal.id;
		delete[] buffer;
	}
}

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

void TerrainModel::makePlaneIndices(unsigned int dimensions)
{
	mesh->indices.resize((dimensions) * (dimensions) * 2 * 3);// 2 triangles per square, 3 indices per triangle dim-1???
		int runner = 0;
	for (int row = 0; row < dimensions - 1; row++)
	{
		for (int col = 0; col < dimensions - 1; col++) //order?
		{
			mesh->indices[runner++] = dimensions * row + col;
			mesh->indices[runner++] = dimensions * row + col + dimensions;
			mesh->indices[runner++] = dimensions * row + col + dimensions + 1;

			mesh->indices[runner++] = dimensions * row + col;
			mesh->indices[runner++] = dimensions * row + col + dimensions + 1;
			mesh->indices[runner++] = dimensions * row + col + 1;
		}
	}
}

void TerrainModel::makePlaneIndices(unsigned int rows,unsigned int columns)
{
	mesh->indices.resize((rows) * (columns) * 2 * 3);// 2 triangles per square, 3 indices per triangle dim-1???
	int runner = 0;
	float MinX = 0, MinZ = 0, MaxX = 0, MaxZ = 0;
	for (int col = 0; col < columns-1; col++)
	{
		for (int row = 0; row < rows-1; row++)
		{
			mesh->indices[runner++] = col * rows + row;
			mesh->indices[runner++] = col * rows + row + rows;
			mesh->indices[runner++] = col * rows + row + rows + 1;

			mesh->indices[runner++] = col * rows + row;
			mesh->indices[runner++] = col * rows + row + rows + 1;
			mesh->indices[runner++] = col * rows + row + 1;
			
			for (int i = 0; i < 6 && runner < mesh->indices.size(); i++) {
				if (mesh->vertices[mesh->indices[runner - i]].Position.x < MinX)
					MinX = mesh->vertices[mesh->indices[runner - i]].Position.x;
				if (mesh->vertices[mesh->indices[runner - i]].Position.x > MaxX)
					MaxX = mesh->vertices[mesh->indices[runner - i]].Position.x;
				if (mesh->vertices[mesh->indices[runner - i]].Position.z < MinZ)
					MinZ = mesh->vertices[mesh->indices[runner - i]].Position.z;
				if (mesh->vertices[mesh->indices[runner - i]].Position.z > MaxZ)
					MaxZ = mesh->vertices[mesh->indices[runner - i]].Position.z;
			}
		}
	}
}

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
	glBindTexture(GL_TEXTURE_2D, m_material.emissive_texture_id); //should be black

	mesh->Draw();
}

size_t TerrainModel::GetVertexCount() const
{
  return mesh->vertices.size();
}

std::vector<const IMesh*> TerrainModel::GetMeshes() const
{
  return std::vector<const IMesh*>{mesh};
}

std::vector<const I3DMesh*> TerrainModel::Get3DMeshes() const
{
	return std::vector<const I3DMesh*>{mesh};
}

std::vector<glm::vec3> TerrainModel::GetPositions() const
{
	std::vector<glm::vec3> ret;
	for (auto& vert : mesh->vertices)
		ret.push_back(vert.Position);
	return ret; // @todo to improve
}

std::vector<GLuint> TerrainModel::GetIndeces() const
{
	return mesh->indices;
}

TerrainModel::~TerrainModel()
{
	if (mesh != nullptr)
		delete mesh;
	//custom normals
 /* normal.freeTexture(); !!!*/
}


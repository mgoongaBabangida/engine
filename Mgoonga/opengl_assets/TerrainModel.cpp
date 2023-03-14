#include "stdafx.h"

#include "TerrainModel.h"
#include "Texture.h"

#include <algorithm>

//----------------------------------------------------------------
TerrainModel::TerrainModel()
	:mesh(nullptr),
  m_diffuse(nullptr),
  m_specular(nullptr),
  m_normal(nullptr),
  m_fourth(nullptr),
  m_height(nullptr)
{}

//----------------------------------------------------------------
TerrainModel::TerrainModel(Texture* diffuse, 
  Texture* specular,
  Texture* normal,
  Texture* heightMap)
  :m_diffuse(diffuse),
  m_specular(specular),
  m_normal(normal),
  m_fourth(diffuse)
{
	mesh = new MyMesh();
	m_size = heightMap->mTextureHeight;
	m_rows = heightMap->mTextureWidth;
	m_columns = heightMap->mTextureHeight;
	m_height = heightMap;
	
	makePlaneVerts(heightMap->mTextureWidth,heightMap->mTextureHeight, false);
	makePlaneIndices(heightMap->mTextureWidth, heightMap->mTextureHeight);
	assignHeights(*m_height);
	generateNormals(heightMap->mTextureWidth, heightMap->mTextureHeight);
	mesh->calculatedTangent();
	mesh->setupMesh();
}

//----------------------------------------------------------------
TerrainModel::TerrainModel(const TerrainModel& other)
  : mesh(other.mesh)
  , m_diffuse(other.m_diffuse)
  , m_specular(other.m_specular)
  , m_normal(other.m_normal)
  , m_fourth(other.m_fourth)
  , m_height(other.m_height)
{}

//----------------------------------------------------------------
void TerrainModel::initialize(Texture* diffuse, Texture* specular, bool spreed_texture)
{
	m_diffuse = diffuse; 
	m_specular = specular;
	m_normal = nullptr;
	m_fourth = diffuse;

  mesh = new MyMesh();
	m_size = m_diffuse->mTextureHeight;
	makePlaneVerts(m_diffuse->mTextureHeight, m_diffuse->mTextureHeight, spreed_texture);
	makePlaneIndices(m_diffuse->mTextureHeight);
	generateNormals(m_size);
	mesh->calculatedTangent();
	mesh->setupMesh();
  mesh->setTextures({ m_diffuse , m_specular , m_normal , m_height });
}

//----------------------------------------------------------------
void TerrainModel::initialize(Texture* diffuse,
														  Texture* specular,
														  Texture* normal,
														  Texture* heightMap,
														  bool spreed_texture)
{
	m_diffuse = diffuse;
	m_specular=specular;
	m_normal=normal;
	m_fourth=diffuse;
	m_height = heightMap;
	
	if(mesh == nullptr)
	 mesh = new MyMesh();

	m_size = heightMap->mTextureHeight;
	m_rows = heightMap->mTextureWidth;
	m_columns = heightMap->mTextureHeight;
	m_height = heightMap;

	makePlaneVerts( heightMap->mTextureWidth, heightMap->mTextureHeight, spreed_texture);
	makePlaneIndices(heightMap->mTextureWidth, heightMap->mTextureHeight);
	assignHeights(*heightMap);
	generateNormals(heightMap->mTextureWidth, heightMap->mTextureHeight);
	
	mesh->calculatedTangent();
	mesh->setupMesh();
  mesh->setTextures({ m_diffuse , m_specular , m_normal , heightMap });
}

//------------------------------------------------------------
TerrainModel::TerrainModel(Texture * color)
  :m_diffuse(color)
  , m_specular(color)
{
	mesh = new MyMesh();
	m_size = 10;
	makePlaneVerts(10);
	makePlaneIndices(10);
	generateNormals(m_size);
	mesh->calculatedTangent();
	mesh->setupMesh();
}

//----------------------------------------------------------------
std::vector<MyMesh*>	TerrainModel::getMeshes()				const 
{ 
  return std::vector<MyMesh*>{ mesh};
}

//------------------------------------------------------------
void					TerrainModel::setDiffuse(Texture* t)
{ m_diffuse = t; }

//------------------------------------------------------------
void					TerrainModel::setSpecular(Texture* t)
{ m_specular = t; }

//----------------------------------------------------------------
 MyVertex TerrainModel::findVertex(float x, float z)
{
	float newX = glm::floor(x);
	newX = newX + (glm::round((x - newX) * devisor)) / devisor;
	float newZ = glm::floor(z);
	newZ = newZ + (glm::round((z - newZ) * devisor)) / devisor;

	auto vert = std::find_if(mesh->vertices.begin(), mesh->vertices.end(), [newX, newZ](const MyVertex& v)
																		   {return v.position.x == newX && v.position.z == newZ; });
	if (vert != mesh->vertices.end())
		return *vert;
	else
		return MyVertex();// std::optional
}

float TerrainModel::GetHeight(float x, float z)
{
	MyVertex vert = findVertex(x, z);
		return vert.position.y;
}

glm::vec3 TerrainModel::GetNormal(float x, float z)
{
	MyVertex vert = findVertex(x, z);
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
		mesh->vertices[i / 4].position.y = (float)buffer[i];
		counter++;
	}
	delete[] buffer;
}

void TerrainModel::generateNormals(GLuint size)
{
	for (unsigned int i = 0; i < mesh->indices.size(); i += 3) 
	{
		glm::vec3& pos1 = mesh->vertices[mesh->indices[i]].position;
		glm::vec3& pos2 = mesh->vertices[mesh->indices[i + 1]].position;
		glm::vec3& pos3 = mesh->vertices[mesh->indices[i + 2]].position;
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
	GLfloat* buffer = new GLfloat[mesh->vertices.size() * 4];
	for (int i = 0; i< mesh->vertices.size(); ++i) {
		buffer[i * 4] = mesh->vertices[i].Normal.x;//(mesh->vertices[i].Normal.x + 1.0f) / 2.0f;  // ( - 0.5) *2;//(GLubyte)128.f;
		buffer[i * 4 + 1] = mesh->vertices[i].Normal.z; //(mesh->vertices[i].Normal.y+1.0f)/2.0f;128
		buffer[i * 4 + 2] = mesh->vertices[i].Normal.y;// (mesh->vertices[i].Normal.z + 1.0f) / 2.0f; //(mesh->vertices[i].Normal.z+1.0f)/2.0f;255
		buffer[i * 4 + 3] = 1.0f;
	}

  m_normal = new Texture(size, size);
	m_normal->TextureFromBuffer(buffer, size, size);
	delete[] buffer;
}

void TerrainModel::generateNormals(GLuint rows, GLuint columns)
{
	for (unsigned int i = 0; i < mesh->indices.size(); i += 3) 
	{
		glm::vec3& pos1 = mesh->vertices[mesh->indices[i]].position;
		glm::vec3& pos2 = mesh->vertices[mesh->indices[i + 1]].position;
		glm::vec3& pos3 = mesh->vertices[mesh->indices[i + 2]].position;
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
	// Load to normal map
	GLfloat* buffer = new GLfloat[mesh->vertices.size() * 4];
	for (int i = 0; i< mesh->vertices.size(); ++i) {
		buffer[i * 4] = (mesh->vertices[i].Normal.x + 1.0f) / 2.0f;
		buffer[i * 4 + 1] = (mesh->vertices[i].Normal.z + 1.0f) / 2.0f;
		buffer[i * 4 + 2] = (mesh->vertices[i].Normal.y + 1.0f) / 2.0f;
		buffer[i * 4 + 3] = 1.0f;
	}

	m_normal = new Texture(rows, columns);
	m_normal->TextureFromBuffer(buffer, rows, columns);
	delete[] buffer;
}

void TerrainModel::makePlaneVerts(unsigned int dimensions, bool spreed_texture)
{
	mesh->vertices.resize(dimensions * dimensions);
	int half = dimensions / 2;
	for (int i = 0; i < dimensions; i++)
	{
		for (int j = 0; j < dimensions; j++)
		{
			MyVertex& thisVert = mesh->vertices[i * dimensions + j];
			thisVert.position.x =(float) (j - half) / devisor;
			thisVert.position.z = (float)(i - half) / devisor;
			thisVert.position.y = 0;
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
			MyVertex& thisVert = mesh->vertices[i * rows + j];
			thisVert.position.x = (float)(j - half_r) / devisor;
			thisVert.position.z = (float)(i - half_c) / devisor;
			thisVert.position.y = 0;
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
			if (thisVert.position.x < MinX)  //debug only
				MinX = thisVert.position.x;
			if (thisVert.position.x > MaxX)
				MaxX = thisVert.position.x;
			if (thisVert.position.z < MinZ)
				MinZ = thisVert.position.z;
			if (thisVert.position.z > MaxZ)
				MaxZ = thisVert.position.z;
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
				if (mesh->vertices[mesh->indices[runner - i]].position.x < MinX)
					MinX = mesh->vertices[mesh->indices[runner - i]].position.x;
				if (mesh->vertices[mesh->indices[runner - i]].position.x > MaxX)
					MaxX = mesh->vertices[mesh->indices[runner - i]].position.x;
				if (mesh->vertices[mesh->indices[runner - i]].position.z < MinZ)
					MinZ = mesh->vertices[mesh->indices[runner - i]].position.z;
				if (mesh->vertices[mesh->indices[runner - i]].position.z > MaxZ)
					MaxZ = mesh->vertices[mesh->indices[runner - i]].position.z;
			}
		}
	}
}

void TerrainModel::Draw()
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_diffuse->id);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_specular->id);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_normal->id);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_fourth->id);

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

std::vector<glm::vec3> TerrainModel::GetPositions() const
{
	std::vector<glm::vec3> ret;
	for (auto& vert : mesh->vertices)
		ret.push_back(vert.position);
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
  if (m_normal != nullptr)
  {
    m_normal->freeTexture();
    delete m_normal;
  }
}


#pragma once

#include "MyModel.h"
#include "Structures.h"
#include "MyMesh.h"

struct Texture;

class ITerrainModel: IModel
{
public:
	virtual float		getHeight(float, float) = 0;
	virtual glm::vec3	getNormal(float, float) = 0;
};

class TerrainModel : public ITerrainModel
{
public:
	TerrainModel():mesh(nullptr), m_diffuse(nullptr), m_specular(nullptr), m_normal(nullptr), m_fourth(nullptr), m_height(nullptr){}
	TerrainModel(Texture* heightMap);  //copy generation in private !!!
	TerrainModel(Texture * diffuse, Texture * specular, Texture * m_normal, Texture * heightMap);
	TerrainModel(const TerrainModel& other);
	virtual ~TerrainModel();

	void initialize(Texture* diffuse, Texture* specular);
	void initialize(Texture* diffuse, Texture* specular, Texture* normal, Texture* heightMap);
	
	float					getHeight(float x , float z)	override;
	glm::vec3				getNormal(float x, float z)		override;
	void					makePlaneIndices(unsigned int rows, unsigned int columns);
	virtual void			Draw()							override;
	std::vector<glm::vec3>	getPositions()			const;
	std::vector<GLuint>		getIndeces()			const;
	vector<MyMesh*>			getMeshes()				const	{ return vector<MyMesh*>{ mesh}; }
	void					setDiffuse(Texture* t)			{ m_diffuse = t;  }
	void					setSpecular(Texture* t)			{ m_specular = t; }

	void					debug();

private:
	unsigned int	devisor = 10;
	MyMesh*			mesh; // generate ourselfs inside constructor
	Texture*		m_diffuse;
	Texture*		m_specular;
	Texture*		m_normal;
	Texture*		m_fourth;
	Texture*		m_height;
	
	GLuint			m_size;
	GLuint			m_rows;
	GLuint			m_columns;

	void			makePlaneVerts(unsigned int dimensions);
	void			makePlaneVerts(unsigned int rows, unsigned int columns);
	void			makePlaneIndices(unsigned int dimensions);
	void			assignHeights(Texture heightMap);
	void			generateNormals(GLuint size);
	void			generateNormals(GLuint rows, GLuint columns);
	MyVertex		findVertex(float x, float z);
};


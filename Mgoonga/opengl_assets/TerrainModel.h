#pragma once

#include <base/base.h>
#include "opengl_assets.h"
#include "MyModel.h"

struct Texture;

//---------------------------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS TerrainModel : public ITerrainModel
{
public:
	TerrainModel();
	TerrainModel(Texture* heightMap);  //copy generation in private !!!
	TerrainModel(Texture * diffuse, Texture * specular, Texture * m_normal, Texture * heightMap);
	TerrainModel(const TerrainModel& other);
	virtual ~TerrainModel();

	void initialize(Texture* diffuse, Texture* specular);
	void initialize(Texture* diffuse, Texture* specular, Texture* normal, Texture* heightMap);
	
	virtual void			Draw()							override;
	float					GetHeight(float x , float z)	override;
	glm::vec3				GetNormal(float x, float z)		override;

	void					makePlaneIndices(unsigned int rows, unsigned int columns);
	
	std::vector<glm::vec3>	GetPositions()			const;
	std::vector<GLuint>		GetIndeces()			const;
	std::vector<MyMesh*>	getMeshes()				const;
	
	void					setDiffuse(Texture* t);
	void					setSpecular(Texture* t);

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


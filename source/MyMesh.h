#pragma once

#include "Texture.h"
#include "Structures.h"
#include "InterfacesDB.h"
#include "ShapeData.h"

using namespace::std;

class MyMesh: public IMesh
{
protected:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	/*  Functions    */
public:
	/*  Mesh Data  */
	vector<MyVertex>	vertices;
	vector<GLuint>		indices;
	vector<Texture>		textures;
	/*  Functions  */
	MyMesh() {  }
	MyMesh(vector< MyVertex> vertices, vector<GLuint> indices, vector<Texture> textures);
	MyMesh(const ShapeData& data);
	
	virtual void Draw()			override;
	virtual void setupMesh();
	virtual void calculatedTangent();
};

class ParticleMesh : public IMesh
{
public:
	/*  Mesh Data  */
	vector< MyVertex>	vertices;
	vector<GLuint>		indices;
	vector<Texture>		textures;
	GLuint				instances;

	static const int		MAXPARTICLES	= 1000;
	static const GLsizei	SIZEOF = sizeof(glm::mat4) + sizeof(glm::vec2) * 3;
	/*  Functions  */
	ParticleMesh(vector< MyVertex> vertices, vector<GLuint> indices, vector<Texture> textures);
	ParticleMesh(const ShapeData & data);

	void				SetUpInstances(GLuint _instances)				{ instances = _instances;  }
	virtual void		Draw() override;
	void				updateInstancedData(std::vector<float>& buffer);

protected:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	GLuint VBOinstanced;
	/*  Functions    */
	virtual void		setupMesh();
};
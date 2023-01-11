#pragma once

#include <base/interfaces.h>

#include "Texture.h"
#include "ShapeData.h"

//----------------------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS MyMesh: public IMesh
{
public:
	MyMesh();
	MyMesh(const MyMesh&) = default;
	virtual ~MyMesh();

	MyMesh(std::vector<MyVertex> vertices, std::vector<GLuint> indices, std::vector<Texture*> textures);
	explicit MyMesh(const ShapeData& data);
	
	virtual void Draw()			override;
  virtual size_t GetVertexCount() const override { return vertices.size(); }
  virtual std::vector<const Texture*> GetTextures() const;

  virtual void setTextures(std::vector<Texture*>);
	virtual void setupMesh();
	virtual void calculatedTangent();

public:
	/*  Mesh Data  */
	std::vector<MyVertex>	vertices;
	std::vector<GLuint>		indices;
	std::vector<Texture*>	textures;

protected:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
};

//----------------------------------------------------------------------------------------------
class ParticleMesh : public IMesh
{
public:
	static const int		MAXPARTICLES	= 1000;
	static const GLsizei	SIZEOF = sizeof(glm::mat4) + sizeof(glm::vec2) * 3;
	
	ParticleMesh(std::vector< MyVertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	ParticleMesh(const ShapeData & data);
	ParticleMesh(const ParticleMesh&) = delete;

  virtual ~ParticleMesh();

	virtual void		Draw() override;
  virtual size_t GetVertexCount() const override { return vertices.size(); }
  virtual std::vector<const Texture*> GetTextures() const;

  void				SetUpInstances(GLuint _instances) { instances = _instances; }
	void				updateInstancedData(std::vector<float>& buffer);

public:
  /*  Mesh Data  */
  std::vector<MyVertex>	vertices;
  std::vector<GLuint>		indices;
  std::vector<Texture>	textures;
  GLuint					instances;

protected:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	GLuint VBOinstanced;
	/*  Functions    */
	virtual void		setupMesh();
};

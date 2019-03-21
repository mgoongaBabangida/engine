#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Structures.h"
#include "Particle.h"
#include "InterfacesDB.h"
#include "Timer.h"
#include "MyMesh.h"

class eObject;
class MyModel;

class eParticleRender
{
public:
	eParticleRender(std::shared_ptr<MyMesh>, Texture*, const string&, const string&);
	
	void	Render(const glm::mat4 &ProjectionMatrix, const Camera &camera);
	void	AddParticleSystem(IParticleSystem* sys);

protected:
	Shader											particleShader;
	std::unique_ptr<MyModel>						model;
	std::unique_ptr<ParticleMesh>					mesh;
	std::unique_ptr<eObject>						object;
	
	std::vector<std::shared_ptr<IParticleSystem> >	systems;
	std::vector<float>								instancedBuffer;

	GLuint	fullTransformationUniformLocation;
	GLuint	modelToWorldMatrixUniformLocation;
	GLuint	worldToProjectionMatrixUniformLocation;
	GLuint	ProjectionMatrixUniformLocation;
	GLuint	modelViewMatrixLocation;
	GLuint	texOffset1Location;
	GLuint	texOffset2Location;
	GLuint	texCoordInfoLocation;

	void	LoadOffsetsInfo(glm::vec2	offset1, 
							glm::vec2	offset2, 
							float		numRows, 
							float		blend);
};

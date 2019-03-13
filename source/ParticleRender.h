#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Structures.h"
#include "Particle.h"
#include "InterfacesDB.h"
#include "Timer.h"

class eObject;
class MyModel;
class ParticleMesh;

class eParticleRender
{
public:
	eParticleRender(MyModel* model, ParticleMesh* mesh, IParticleSystem* sys, ParticleSystemInfo info);
	
	void	Render(const glm::mat4 &ProjectionMatrix, const Camera &camera);
	void	AddParticleSystem(IParticleSystem* sys, ParticleSystemInfo info);

protected:
	Shader											particleShader;
	ParticleMesh*									mesh;//uniq?
	eObject*										object;//uniq?
	std::vector<std::shared_ptr<IParticleSystem> >	systems;
	std::vector<ParticleSystemInfo>					infos;
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

#pragma once

#include "Shader.h"
#include "Texture.h"
#include <math/Camera.h>
#include <math/Particle.h>
#include <base/interfaces.h>
#include <math/Timer.h>
#include "MyMesh.h"

#include <chrono>
#include <memory>
#include <vector>

class eObject;
class MyModel;

//------------------------------------------------------------------------------------
class eParticleRender
{
public:
	eParticleRender(std::shared_ptr<MyMesh>, const Texture*, const std::string&, const std::string&);
	~eParticleRender();

	void	Render(const Camera &camera);
	void	AddParticleSystem(std::shared_ptr<IParticleSystem> sys);

	Shader& GetShader() { return particleShader; }

protected:
	Shader											        particleShader;
	std::unique_ptr<MyModel>						model;
	std::unique_ptr<ParticleMesh>				mesh;
	std::unique_ptr<eObject>						object;
	
	std::vector<std::shared_ptr<IParticleSystem> >	systems;
	std::vector<float>								              instancedBuffer;

	GLuint	fullTransformationUniformLocation;
	GLuint	modelToWorldMatrixUniformLocation;
	GLuint	worldToProjectionMatrixUniformLocation;
	GLuint	ProjectionMatrixUniformLocation;
	GLuint	modelViewMatrixLocation;
	GLuint	texOffset1Location;
	GLuint	texOffset2Location;
	GLuint	texCoordInfoLocation;
};

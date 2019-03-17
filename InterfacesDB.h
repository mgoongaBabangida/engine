#ifndef  INTERFACESDB_H
#define  INTERFACESDB_H

#include <glew-2.0.0\include\GL\glew.h>

#include <memory>
#include <string>
#include <vector>
#include "Texture.h"

#include <glm\glm\glm.hpp>
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>

class Particle;

class IParticleSystem
{
public:
	IParticleSystem(Texture* t, float s) : texture(t), scale(s, s, s) {}
	static const int MAX_PARTICLES = 1000;
	virtual ~IParticleSystem() = default;

	virtual void							generateParticles()							= 0;
	virtual std::vector<Particle>::iterator prepareParticles(glm::vec3 cameraPosition)	= 0;
	virtual std::vector<Particle>&			getParticles()								= 0;
	virtual bool							IsFinished()								= 0;
	// to improve
	glm::vec3								Scale()			{ return scale; }
	Texture*								GetTexture()	{ return texture; }
protected:
	Texture*								texture;
	glm::vec3								scale;
};

class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual void execute() = 0;

};

struct Collission;
class eObject;

class IScript
{
protected:
	eObject* object = nullptr;
public:
	virtual ~IScript() = default;
	virtual void	ReactCollision(const Collission& col) {}
	virtual void	Update(std::vector< std::shared_ptr<eObject> > objs) = 0;
	virtual void	setDestination(glm::vec3) = 0;
	virtual void	shoot() = 0;
	void			setObject(eObject* obj) { object = obj; }
};

class IMesh
{
public:
	virtual ~IMesh() = default;
	virtual void Draw() = 0;
};

class IModel
{
public:
	virtual ~IModel() = default;
	virtual void					Draw()					= 0;
	virtual std::vector<glm::vec3>	getPositions()	const	= 0;
	virtual std::vector<GLuint>		getIndeces()	const	= 0;
};

#endif

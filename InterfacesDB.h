#ifndef  INTERFACESDB_H
#define  INTERFACESDB_H

#include <glew-2.1.0\include\GL\glew.h>

#include <memory>
#include <string>
#include <vector>
#include "Structures.h"
#include "Texture.h"

#include <glm\glm\glm.hpp>
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>

class  Particle;
class  eObject;

class IInputObserver
{
public:
	virtual bool OnMouseMove(uint32_t x, uint32_t y)				{ return false; }
	virtual bool OnKeyPress(uint32_t asci)							{ return false; }
	virtual bool OnMousePress(uint32_t x, uint32_t y, bool left)	{ return false; }
	virtual bool OnMouseRelease()									{ return false; }
};

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

class IScript : public IInputObserver
{
protected:
	eObject* object = nullptr;
public:
	virtual ~IScript() = default;
	virtual void	Update(std::vector< std::shared_ptr<eObject> > objs) = 0;
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

#ifndef  INTERFACESDB_H
#define  INTERFACESDB_H

//PARTICLE SYSTEM PACKAGE
class Particle;
struct Texture;

class IParticleSystem
{
public:
	static const int MAX_PARTICLES = 1000;
	virtual ~IParticleSystem() {}

	virtual void							generateParticles(glm::vec3 systemCenter, Texture* texture) = 0;
	virtual std::vector<Particle>::iterator prepareParticles(glm::vec3 cameraPosition) = 0;
	virtual std::vector<Particle>&			getParticles() = 0;
};

class ICommand
{
public:
	virtual void execute() = 0;

};

struct Collission;
class eObject;

class IScript
{
protected:
	eObject* object = nullptr;
public:
	virtual void	ReactCollision(const Collission& col) {}
	virtual void	Update(std::vector< std::shared_ptr<eObject> > objs) = 0;
	virtual void	setDestination(glm::vec3) = 0;
	virtual void	shoot() = 0;
	void			setObject(eObject* obj) { object = obj; }
};

class IMesh
{
public:
	virtual void Draw() = 0;
};

class IModel
{
public:
	virtual void					Draw()					= 0;
	virtual std::vector<glm::vec3>	getPositions()	const	= 0;
	virtual std::vector<GLuint>		getIndeces()	const	= 0;
};

#endif

#ifndef  INTERFACES_H
#define  INTERFACES_H

#include "base.h"

#include <memory>
#include <string>
#include <vector>

#include <glm\glm\glm.hpp>
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>

class Particle;
struct Texture;

//----------------------------------------------------------------------------------------------
class IInputObserver
{
public:
	virtual ~IInputObserver() = default;
	virtual bool OnMouseMove(uint32_t x, uint32_t y)				{ return false; }
	virtual bool OnKeyPress(uint32_t asci)							{ return false; }
	virtual bool OnMousePress(uint32_t x, uint32_t y, bool left)	{ return false; }
	virtual bool OnMouseRelease()									{ return false; }
};

//----------------------------------------------------------------------------------------------
class IParticleSystem
{
public:
	IParticleSystem(Texture* t, float s) 
		: texture(t), 
		  scale(s, s, s) {}
	virtual ~IParticleSystem() {}

	static const int MAX_PARTICLES = 1000;

	virtual void							GenerateParticles()							= 0;
	virtual std::vector<Particle>::iterator PrepareParticles(glm::vec3 cameraPosition)	= 0;
	virtual std::vector<Particle>&			GetParticles()								= 0;
	virtual bool							IsFinished()								= 0;
	// to improve
	glm::vec3								Scale()			{ return scale; }
	Texture*								GetTexture()	{ return texture; }

protected:
	Texture*								texture;
	glm::vec3								scale;
};

//----------------------------------------------------------------------------------------------
class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual void Execute() = 0;
};

//----------------------------------------------------------------------------------------------
class IScript : public IInputObserver
{
protected:
	eObject* object = nullptr;
public:
	virtual ~IScript() = default;
	virtual void	                    Update(std::vector<std::shared_ptr<eObject>> objs) = 0;
	virtual void	                    CollisionCallback(const eCollision&)				{}
  virtual std::vector<eObject*>			GetChildrenObjects() { return std::vector<eObject*>{}; }
	void			                        SetObject(eObject* obj) { object = obj; }
};

//-----------------------------------------------------------------------------
class IAnimation
{
public:
  virtual ~IAnimation() = default;

  virtual void Start() = 0;
  virtual void Stop() = 0;
  virtual void Continue() = 0;
  virtual bool IsPaused() = 0;
  virtual const std::string& Name() const = 0;
};

//----------------------------------------------------------------------------------------------
class IMesh
{
public:
	virtual ~IMesh() = default;
	virtual void Draw() = 0;
  virtual size_t GetVertexCount() const = 0;
  virtual std::vector<const Texture*> GetTextures() const = 0;
};

//----------------------------------------------------------------------------------------------
class IModel
{
public:
	virtual ~IModel() = default;
	virtual void						          Draw()					= 0;
	virtual std::vector<glm::vec3>		GetPositions()	const	= 0;
	virtual std::vector<unsigned int>	GetIndeces()	const	= 0;
  virtual size_t                    GetVertexCount() const = 0;
  virtual size_t                    GetMeshCount() const = 0;
  virtual std::vector<const IMesh*> GetMeshes() const = 0;
  virtual size_t                    GetAnimationCount() const = 0;
  virtual std::vector<const IAnimation*> GetAnimations() const = 0;
  virtual std::vector<const Texture*> GetTexturesModelLevel() const {
    return std::vector<const Texture*>();
  } //$todo delete later
};

//----------------------------------------------------------------------------------------------
class ITerrainModel : public IModel
{
public:
	virtual ~ITerrainModel() = default;
	virtual float		GetHeight(float, float) = 0;
	virtual glm::vec3	GetNormal(float, float) = 0;
};

//-----------------------------------------------------------------------------------------------
class ITransform
{
public:
	virtual ~ITransform() = default;
	virtual glm::mat4	getModelMatrix() const = 0;
	virtual void		setRotation(float x, float y, float z) = 0;
	virtual void		setRotation(glm::quat q) = 0;
	virtual void		setTranslation(glm::vec3 tr) = 0;
	virtual void		setScale(glm::vec3 sc) = 0;
	virtual bool		isRotationValid() = 0;

	virtual glm::mat4	  getScale()			const = 0;
  virtual glm::vec3   getScaleAsVector() const = 0;
	virtual glm::vec3	  getTranslation()	const = 0;
	virtual glm::vec3&	getTranslationRef() = 0;
	virtual glm::quat	  getRotation()		const = 0;
	virtual glm::vec4   getRotationVector() const = 0;
	virtual glm::vec3   getForward()		const = 0;
	virtual glm::vec3   getUp()				const = 0;
	virtual void		    incrementScale() = 0;
	virtual void		    decrementScale() = 0;
	virtual void		    billboard(glm::vec3 direction) = 0;
	virtual bool		    turnTo(glm::vec3 dest, float speed) = 0;
};

//----------------------------------------------------------------------------------------------
class ICollider
{
public:
	virtual ~ICollider() = default;
	virtual void CalculateExtremDots(const std::vector<glm::vec3>& positions) = 0;
	virtual bool CollidesWith(const ITransform& trans1,
							  const ITransform& trans2,
							  const ICollider& other,
							  Side moveDirection,
							  eCollision& collision) = 0;

	virtual float getMaxX()const = 0;
	virtual float getMaxY()const = 0;
	virtual float getMaxZ()const = 0;
	virtual float getMinX()const = 0;
	virtual float getMinY()const = 0;
	virtual float getMinZ()const = 0;

	virtual glm::vec3				GetCenter() const = 0;
	virtual std::vector<glm::mat3>	GetBoundingTriangles(const ITransform& trans)const = 0;
	virtual std::vector<glm::vec3>	GetExtrems(const ITransform& trans) const = 0;
};

//----------------------------------------------------------------------------------------------
class IRigger
{
public:
	virtual ~IRigger() = default;
	virtual bool							Apply(const std::string& _animation)								 = 0;
  virtual bool							Apply(size_t _animation_index) = 0;
	virtual void							Stop()																 = 0;
	virtual bool							ChangeName(const std::string& _oldName, const std::string& _newName) = 0;
	virtual const std::vector<glm::mat4>&	GetMatrices()														 = 0;
};

//-----------------------------------------------------------------------------------------------
class IRigidBody
{
public:
	virtual ~IRigidBody() = default;

	virtual void SetObject(eObject* obj) =0;

	virtual void TurnRight(std::vector<std::shared_ptr<eObject> > objects) = 0;
	virtual void TurnLeft(std::vector<std::shared_ptr<eObject> > objects) = 0;
	virtual void LeanRight(std::vector<std::shared_ptr<eObject> > objects) = 0;
	virtual void LeanLeft(std::vector<std::shared_ptr<eObject> > objects) = 0;
	virtual void LeanForward(std::vector<std::shared_ptr<eObject> > objects) = 0;
	virtual void LeanBack(std::vector<std::shared_ptr<eObject> > objects) = 0;

	virtual void MoveForward(std::vector<std::shared_ptr<eObject> > objects) = 0;
	virtual void MoveBack(std::vector<std::shared_ptr<eObject> > objects) = 0;
	virtual void MoveLeft(std::vector<std::shared_ptr<eObject> > objects) = 0;
	virtual void MoveRight(std::vector<std::shared_ptr<eObject> > objects) = 0;
	virtual void MoveUp(std::vector<std::shared_ptr<eObject> > objects) = 0;
	virtual void MoveDown(std::vector<std::shared_ptr<eObject> > objects) = 0;

	virtual void Move(std::vector<std::shared_ptr<eObject> > objects) = 0;
	virtual void Turn(glm::vec3 direction, std::vector<std::shared_ptr<eObject>> objects) = 0;

	virtual void		 SetCurrentVelocity(glm::vec3 _vel) = 0;
	virtual glm::vec3	 Velocity()	const = 0;
};

//---------------------------------------------------------------------------
class ISound
{
public:
  virtual ~ISound() = default;
	virtual void Play()		 = 0;
	virtual bool isPlaying() = 0;
	virtual void Stop()		 = 0;
};

#endif


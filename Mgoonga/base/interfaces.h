#ifndef  INTERFACES_H
#define  INTERFACES_H

#include "base.h"

#include <optional>

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
	virtual bool OnKeyPress(uint32_t asci) { return false; }

	virtual bool OnMouseMove(int32_t x, int32_t y)				{ return false; }
	virtual bool OnMousePress(int32_t x, int32_t y, bool left)	{ return false; }
	virtual bool OnMouseRelease()									{ return false; }
	virtual bool OnMouseWheel(int32_t x, int32_t y) { return false; }
};

//----------------------------------------------------------------------------------------------
class IParticleSystem
{
public:
	IParticleSystem(const Texture* t)
		: texture(t) {}
	virtual ~IParticleSystem() {}

	static const int MAX_PARTICLES = 1000;

	virtual void								            Start() = 0;
	virtual void														GenerateParticles()			= 0;
	virtual std::vector<Particle>::iterator PrepareParticles(glm::vec3 cameraPosition)	= 0;
	virtual std::vector<Particle>&					GetParticles()	= 0;
	virtual bool														IsFinished()		= 0;
	virtual bool								            IsStarted()			= 0;
	virtual void								            Reset()					= 0;

	virtual float& ConeAngle() = 0;
	virtual glm::vec3& Scale()	= 0;
	virtual float& Speed() = 0;
	virtual float& BaseRadius() = 0;
	virtual float& LifeLength() = 0;
	virtual int& ParticlesPerSecond() = 0;
	virtual bool& Loop() = 0;
	virtual float& Gravity() = 0;

	//@todo improve
	const Texture*					GetTexture() const	{ return texture; }

protected:
	const Texture*					texture;
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
	virtual void																			Initialize() {}
	virtual void																			Update(float _tick) = 0;
	virtual void																			CollisionCallback(const eCollision&)	{}
	void																							SetObject(eObject* obj) { object = obj; }
	const eObject*																		GetScriptObject() const { return object; }
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
	virtual void															Draw() = 0;
	virtual const std::string&								Name() const = 0;

	virtual bool											HasMaterial() const	{ return false; }
	virtual void											SetMaterial(const Material&) {}
	virtual std::optional<Material>		GetMaterial() const { return std::nullopt; }
};

//----------------------------------------------------------------------------------------------
class I3DMesh : public IMesh
{
public:
	virtual ~I3DMesh() = default;
	virtual size_t														GetVertexCount() const	= 0;
	virtual const std::vector<Vertex>&				GetVertexs() const			= 0;
	virtual const std::vector<unsigned int>&	GetIndices() const			= 0;
	virtual std::vector<TextureInfo>					GetTextures() const			= 0;
	virtual void AddTexture(Texture*)																	= 0;
	virtual void BindVAO()	const																			= 0;
	virtual void UnbindVAO() const																		= 0;
	virtual void DrawInstanced(int32_t) {}
};

//----------------------------------------------------------------------------------------------
class IBone
{
public:
	virtual ~IBone() = default;

	virtual const std::string& GetName()  const							= 0;
	virtual size_t GetID() const														= 0;
	virtual  std::vector<const IBone*> GetChildren() const	= 0;
	virtual const glm::mat4& GetLocalBindTransform() const	= 0;
	virtual const glm::mat4& GetMTransform() const					= 0;
	virtual bool IsRealBone() const													= 0;
};

//----------------------------------------------------------------------------------------------
class IModel
{
public:
	virtual ~IModel() = default;

	virtual void														Draw()					= 0;
	virtual void														DrawInstanced(int32_t) {}

	virtual const std::string&							GetName() const = 0;
	virtual const std::string&							GetPath() const = 0;

	virtual size_t													GetVertexCount() const = 0;
  virtual size_t													GetMeshCount() const = 0;
  virtual std::vector<const IMesh*>				GetMeshes() const = 0;
	virtual std::vector<const I3DMesh*>			Get3DMeshes() const = 0;

	virtual bool														HasBones() const = 0;
	virtual std::vector<const IBone*>				GetBones() const = 0;

  virtual size_t													GetAnimationCount() const = 0;
  virtual std::vector<const IAnimation*>	GetAnimations() const = 0;

	virtual bool											HasMaterial() const { return false; }
	virtual void											SetMaterial(const Material&) {}
	virtual std::optional<Material>		GetMaterial() const { return std::nullopt; }
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

	virtual void	setRotation(float x, float y, float z) = 0;
	virtual void	setRotation(glm::quat q) = 0;
	virtual void	setTranslation(glm::vec3 tr) = 0;
	virtual void	setScale(glm::vec3 sc) = 0;
	virtual void	setUp(glm::vec3 _up) = 0;
	virtual void	setForward(glm::vec3 _fwd) = 0;
	virtual void	setModelMatrix(const glm::mat4&) = 0;

	virtual const glm::mat4&	getModelMatrix() const = 0;
	virtual glm::mat4					getScale()			const = 0;
  virtual glm::vec3					getScaleAsVector() const = 0;
	virtual glm::vec3					getTranslation()	const = 0;
	virtual glm::vec3&				getTranslationRef() = 0;
	virtual glm::quat					getRotation()		const = 0;
	virtual glm::vec4					getRotationVector() const = 0;
	virtual glm::vec4					getRotationUpVector() const = 0;
	virtual glm::vec3					getUp()				const = 0;
	virtual glm::vec3					getForward()		const = 0;

	virtual void		incrementScale() = 0;
	virtual void		decrementScale() = 0;
	virtual void		billboard(glm::vec3 direction) = 0;
	virtual bool		turnTo(glm::vec3 dest, float speed) = 0;
	virtual bool		isRotationValid() = 0;
};

//----------------------------------------------------------------------------------------------
class ICollider
{
public:
	virtual ~ICollider() = default;
	virtual void CalculateExtremDots(const eObject* _object) = 0;
	virtual bool CollidesWith(const ITransform& trans1,
							  const ITransform& trans2,
							  const ICollider& other,
							  Side moveDirection,
							  eCollision& collision) = 0;
	virtual bool CollidesWith(const ITransform& _trans,
														std::vector<std::shared_ptr<eObject>> _objects,
														Side _moveDirection,
														eCollision& _collision) = 0;

	virtual bool IsInsideOfAABB(const ITransform& _trans, const ITransform& trans2, const ICollider& other) = 0;

	virtual glm::vec3								GetCenter() = 0;
	virtual float										GetRadius() = 0;
	virtual const std::string&			GetModelName() = 0;
	virtual const std::string&			GetPath() const = 0;
	virtual void										SetPath(const std::string&) = 0;

	virtual std::vector<glm::mat3>	GetBoundingTriangles(const ITransform& trans)const = 0;
	virtual std::vector<glm::vec3>	GetExtrems(const ITransform& trans) const = 0;

	virtual std::vector<glm::mat3>	GetBoundingTrianglesLocalSpace()const = 0;
	virtual std::vector<glm::vec3>	GetExtremsLocalSpace() const = 0;
	virtual extremDots							GetExtremDotsLocalSpace() const = 0;
};

//----------------------------------------------------------------------------------------------
class IRigger
{
public:
	virtual ~IRigger() = default;
	virtual bool													Apply(const std::string& _animation, bool _play_once)																		= 0;
  virtual bool													Apply(size_t _animation_index, bool _play_once)																					= 0;
	virtual void													Stop()																																									= 0;
	virtual bool													ChangeName(const std::string& _oldName, const std::string& _newName)										= 0;
	virtual void													CreateSocket(const std::shared_ptr<eObject>& _socket_obj, const std::string& _boneName) = 0;
	virtual std::vector<AnimationSocket>	GetSockets() const																																			= 0;

	virtual const std::vector<glm::mat4>&	GetMatrices()																																						= 0;
	virtual size_t												GetAnimationCount() const																																= 0;
	virtual IAnimation*										GetCurrentAnimation() const																															= 0;
	virtual std::vector<std::string>			GetAnimationNames() const																																= 0;
	virtual const std::string&						GetCurrentAnimationName() const																													= 0;
	virtual size_t												GetCurrentAnimationFrameIndex() const																										= 0;
	virtual size_t												GetBoneCount() const																																		= 0;
	virtual std::vector<glm::mat4>				GetMatrices(const std::string& _animationName, size_t _frame)														= 0;
	virtual const std::string&						GetPath() const																																					= 0;
	virtual void													SetPath(const std::string&)																															= 0;
};

//-----------------------------------------------------------------------------------------------
class IRigidBody
{
public:
	virtual ~IRigidBody() = default;

	virtual void SetObject(eObject* obj) =0;

	virtual bool Update(float _dt, std::vector<std::shared_ptr<eObject> > objects) = 0;

	virtual void ApplyForce(glm::vec3 _force) = 0;
	virtual void ApplyForce(glm::vec3 _direction, float _magitude) = 0;

	virtual void ApplyAcceleration(glm::vec3 _acceleration) = 0;
	virtual void ApplyAcceleration(glm::vec3 _direction, float _magitude) = 0;

	virtual void ApplyImpulse(glm::vec3 _force, float _dt) = 0;
	virtual void ApplyImpulse(glm::vec3 _direction, float _magitude, float _dt) = 0;

	virtual void TransferEnergy(float _joules) = 0;

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

	virtual void Move(std::vector<std::shared_ptr<eObject> > objects, float _dt = 0.0f) = 0;
	virtual void Turn(glm::vec3 direction, std::vector<std::shared_ptr<eObject>> objects) = 0;

	virtual void				SetCurrentVelocity(glm::vec3 _vel) = 0;
	virtual glm::vec3		Velocity()	const = 0;
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

//-----------------------------------------------------------------------------
class IGame
{
public:
	virtual ~IGame() = default;
	virtual void		InitializeGL() = 0;
	virtual void		PaintGL() = 0;
	virtual uint32_t GetFinalImageId() = 0;

	virtual std::shared_ptr<eObject> GetFocusedObject() = 0; //const ref?
	virtual std::vector<std::shared_ptr<eObject>> GetObjects() = 0;

	virtual void AddObject(std::shared_ptr<eObject>) = 0;
	virtual void DeleteObject(std::shared_ptr<eObject>) = 0;

	virtual void SetFocused(std::shared_ptr<eObject>) = 0;

	virtual const Texture* GetTexture(const std::string& _name) const = 0;
	virtual Light& GetMainLight() = 0;

	virtual glm::mat4 GetMainCameraViewMatrix() = 0;
	virtual glm::mat4 GetMainCameraProjectionMatrix() = 0;

	virtual glm::vec3 GetMainCameraPosition() const = 0;
	virtual glm::vec3 GetMainCameraDirection() const = 0;

	virtual void AddInputObserver(IInputObserver* _observer, ePriority _priority) = 0;
	virtual void DeleteInputObserver(IInputObserver* _observer) = 0;

	virtual bool UseGizmo() = 0;
	virtual uint32_t CurGizmoType() = 0;

	virtual size_t			Width() const = 0 ;
	virtual size_t			Height() const = 0 ;
};

#endif


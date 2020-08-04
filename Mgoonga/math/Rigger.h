#pragma once

#include <base/interfaces.h>

#include "math.h"
#include "Timer.h"
#include "Bone.h"
#include "SceletalAnimation.h"

#include <string>
#include <atomic>

class eAnimatedModel;
struct Raii;

//----------------------------------------------------------------------------------------------------
class DLL_MATH Rigger : public IRigger
{
	friend struct eRaii;
public:
	explicit Rigger(eAnimatedModel* _model);
	virtual bool							Apply(const std::string& _animation);
	virtual void							Stop();
	virtual bool							ChangeName(const std::string& _oldName, const std::string& _newName);
	virtual const std::vector<glm::mat4>&	GetMatrices();

protected:
	void							UpdateAnimation(Bone &bone, const Frame &frame, const glm::mat4 &ParentTransform);

	/*const*/ eAnimatedModel*		model;
	std::vector<SceletalAnimation>	animations;
	SceletalAnimation*				currentAnim = nullptr;
	std::vector<Bone>				bones;
	std::string						nameRootBone;
	std::vector<glm::mat4>			matrices;
	std::unique_ptr<math::Timer>	timer;
	std::atomic<bool>				matrix_flag = false;
};

struct eRaii
{
	eRaii(Rigger* _r): r(_r){}
	~eRaii()				{ bool tr = true; r->matrix_flag.compare_exchange_weak(tr, false); }
	Rigger* r;
};
#pragma once

#include <base/interfaces.h>

#include "math.h"
#include "Timer.h"
#include "Bone.h"
#include "SceletalAnimation.h"

#include <atomic>

class eAnimatedModel;
struct Raii;

//----------------------------------------------------------------------------------------------------
class DLL_MATH Rigger : public IRigger
{
	friend struct eRaii;
public:
	explicit									Rigger(eAnimatedModel* _model);
	virtual										~Rigger();
	virtual bool							Apply(const std::string& _animation, bool _play_once = false);
  virtual bool							Apply(size_t _animation_index, bool _play_once = false);
	virtual void							Stop();
	virtual bool							ChangeName(const std::string& _oldName, const std::string& _newName);
	virtual const std::vector<glm::mat4>&	GetMatrices();
	virtual std::vector<glm::mat4> GetMatrices(const std::string& _animationName, size_t _frame);
	virtual size_t						GetAnimationCount() const;
	virtual size_t						GetBoneCount() const;
	const std::string&				GetCurrentAnimationName() const;
	virtual size_t						GetCurrentAnimationFrameIndex() const;
	SceletalAnimation*				GetCurrentAnimation() const { return currentAnim; }

	void AddAnimations(std::vector<SceletalAnimation>);
	std::vector<std::string> GetAnimationNames() const;

	void SetActiveBoneIndex(int32_t _index) { active_bone_index = _index; }
	int32_t GetActiveBoneIndex() { return active_bone_index; }

	std::vector<std::string> GetBoneNames() const;
	glm::mat4 GetBindMatrixForBone(const std::string& _boneName) const;
	glm::mat4 GetBindMatrixForBone(size_t _boneID) const;
	glm::mat4 GetCurrentMatrixForBone(const std::string& _boneName) const;
	glm::mat4 GetCurrentMatrixForBone(size_t _boneID) const;

protected:
	void							UpdateAnimation(Bone &bone, const Frame &frame, const glm::mat4 &ParentTransform);

	/*const*/ eAnimatedModel*				model;
	std::vector<SceletalAnimation>	animations;
	SceletalAnimation*							currentAnim = nullptr;
	std::vector<Bone>								bones;
	std::string											nameRootBone;
	int32_t													active_bone_index = MAX_BONES;
	std::vector<glm::mat4>					matrices;
	std::unique_ptr<math::Timer>		timer;

	std::atomic<bool>								matrix_flag = false;
	std::mutex											mutex;
	std::condition_variable					cv;
	bool														is_active = true;
};

struct eRaii
{
	eRaii(Rigger* _r): r(_r){}
	~eRaii()				{ bool tr = true; r->matrix_flag.compare_exchange_weak(tr, false); }
	Rigger* r;
};
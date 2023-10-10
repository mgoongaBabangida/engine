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
	Rigger(std::vector<SceletalAnimation>, std::vector<Bone>, std::string, const glm::mat4&, const std::string& _path = "");

	virtual										~Rigger();

	virtual bool													Apply(const std::string& _animation, bool _play_once = false);
  virtual bool													Apply(size_t _animation_index, bool _play_once = false);
	virtual void													Stop();
	virtual bool													ChangeName(const std::string& _oldName, const std::string& _newName);

	virtual void													CreateSocket(const std::shared_ptr<eObject>& _socket_obj, const std::string& _boneName)  override;
	virtual std::vector<AnimationSocket>	GetSockets() const override;

	virtual const std::vector<glm::mat4>&	GetMatrices();
	virtual std::vector<glm::mat4>				GetMatrices(const std::string& _animationName, size_t _frame);
	virtual size_t												GetAnimationCount() const;
	virtual size_t												GetBoneCount() const;
	const std::string&										GetCurrentAnimationName() const;
	virtual size_t												GetCurrentAnimationFrameIndex() const;

	virtual const std::string&						GetPath() const { return m_path; }
	virtual void													SetPath(const std::string& _path) { m_path = _path; }

	virtual bool													UseFirstFrameAsIdle() override;

	SceletalAnimation*										GetCurrentAnimation() const { return currentAnim; }
	void																	SetCurrentAnimation(const std::string& _animationName);

	void											AddAnimations(std::vector<SceletalAnimation>);
	std::vector<std::string>	GetAnimationNames() const;

	void		SetActiveBoneIndex(int32_t _index) { active_bone_index = _index; }
	int32_t GetActiveBoneIndex() { return active_bone_index; }

	const Bone* GetParent(const std::string& _boneName);
	const std::vector<Bone*> GetChildren(const std::string& _boneName);

	std::vector<std::string>	GetBoneNames() const;
	glm::mat4									GetBindMatrixForBone(const std::string& _boneName) const;
	glm::mat4									GetBindMatrixForBone(size_t _boneID) const;

	glm::mat4 GetCurrentMatrixForBone(const std::string& _boneName) const;
	glm::mat4 GetCurrentMatrixForBone(size_t _boneID) const;

	const std::vector<SceletalAnimation>& GetAnimations() const						{ return animations; }
	const std::vector<Bone>&							GetBones() const								{ return bones; }
	const std::string&										GetNameRootBone() const					{ return nameRootBone; }
	const glm::mat4&											GetGlobalModelTransform() const { return globalModelTransform; }

protected:
	void							_UpdateAnimation(Bone &bone, const Frame &frame, const glm::mat4 &ParentTransform);
	void							_Initialize();

	std::vector<SceletalAnimation>	animations;
	std::vector<Bone>								bones;
	std::vector<AnimationSocket>		m_sockets;

	std::string											nameRootBone;
	glm::mat4												globalModelTransform;

	SceletalAnimation*							currentAnim = nullptr;
	int32_t													active_bone_index = MAX_BONES;

	std::vector<glm::mat4>					matrices;
	std::unique_ptr<math::Timer>		timer;

	std::atomic<bool>								matrix_flag = false;
	std::mutex											mutex;
	std::condition_variable					cv;
	std::atomic<bool>								is_active = true;

	std::string											m_path;
};

struct eRaii
{
	eRaii(Rigger* _r): r(_r){}
	~eRaii()				{ bool tr = true; r->matrix_flag.compare_exchange_weak(tr, false); }
	Rigger* r;
};
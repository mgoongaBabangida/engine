#include "stdafx.h"
#include "Rigger.h"
#include "AnimatedModel.h"

#include <algorithm>>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iterator>

//-------------------------------------------------------------------------------------------
Rigger::Rigger(eAnimatedModel* _model)
{
	if(_model)
	{
		animations		=					_model->Animations();
		bones			=							_model->Bones();
		nameRootBone	=					_model->RootBoneName();
		globalModelTransform =	_model->GlobalTransform();

		_Initialize();
	}
}

//-------------------------------------------------------------------------------------------
Rigger::Rigger(std::vector<SceletalAnimation> _animations,
							 std::vector<Bone> _bones,
							 std::string _rootBoneName,
							 const glm::mat4& _globaltransform,
							 const std::string& _path)
: animations(_animations)
, bones(_bones)
, nameRootBone(_rootBoneName)
, globalModelTransform(_globaltransform)
, m_path(_path)
{
	_Initialize();
}

//-------------------------------------------------------------------------------------------
Rigger::~Rigger()
{
	is_active = false; // no more sleep
	cv.notify_one(); // wake up
	timer->stop();
}

//-------------------------------------------------------------------------------------------
bool Rigger::Apply(const std::string & _animation, bool _play_once)
{
	if (currentAnim != nullptr && currentAnim->Name() == _animation)
	{
		{
			std::lock_guard lk(mutex);
			if (_play_once)
				currentAnim->PlayOnce();
			else
				currentAnim->Continue();
		}
		cv.notify_one();
		return true;
	}
	else
	{
		auto anim = std::find_if(animations.begin(), animations.end(),
			[_animation](const SceletalAnimation& anim) { return anim.Name() == _animation; });
		if(anim != animations.end())
		{
			{
				std::lock_guard lk(mutex);
				currentAnim = &(*anim);
				if (_play_once)
					currentAnim->PlayOnce();
				else
					currentAnim->Start();
			}
			cv.notify_one();
			return true;
		}
		else
		{
			return false;
		}				
	}
}

//-------------------------------------------------------------------------------------------
bool Rigger::Apply(size_t _animation_index, bool _play_once)
{
  if (currentAnim != nullptr && currentAnim == &animations[_animation_index])
  {
		{
			std::lock_guard lk(mutex);
			if (_play_once)
				currentAnim->PlayOnce();
			else
				currentAnim->Continue();
		}
		cv.notify_one();
    return true;
  }
  else if(_animation_index < animations.size())
  {
		{
			std::lock_guard lk(mutex);
			currentAnim = &animations[_animation_index];
			if (_play_once)
				currentAnim->PlayOnce();
			else
				currentAnim->Start();
		}
		cv.notify_one();
    return true;
  }
  else
    return false;
}

//-------------------------------------------------------------------------------------------
void Rigger::SetCurrentAnimation(const std::string& _animationName)
{
	auto anim = std::find_if(animations.begin(), animations.end(),
		[_animationName](const SceletalAnimation& anim) { return anim.Name() == _animationName; });
	if (anim != animations.end())
	{
		std::lock_guard lk(mutex);
		currentAnim = &(*anim);
	}
}

//-------------------------------------------------------------------------------------------
void Rigger::Stop()
{
	currentAnim->Stop();
}

//-------------------------------------------------------------------------------------------
bool Rigger::ChangeName(const std::string & _oldName, const std::string & _newName)
{
	auto anim = std::find_if(animations.begin(), animations.end(),
		[_oldName](SceletalAnimation anim) { return anim.Name() == _oldName; });
	if (anim != animations.end())
	{
		anim->SetName(_newName);
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------
const std::vector<glm::mat4>& Rigger::GetMatrices()
{
	eRaii raii(this);
	bool fls = false;
	while(!matrix_flag.compare_exchange_weak(fls, true)) {}
	return matrices;
}

//-------------------------------------------------------------------------------------------
std::vector<glm::mat4> Rigger::GetMatrices(const std::string& _animationName, size_t _frame)
{
	auto anim = std::find_if(animations.begin(), animations.end(),
		[_animationName](SceletalAnimation anim) { return anim.Name() == _animationName; });
	if(anim == animations.end() || anim->GetNumFrames() < _frame)
		return {};
	else
	{
		std::vector<glm::mat4> ret(bones.size());
		auto root = std::find_if(bones.begin(), bones.end(), [this](const Bone& bone) { return nameRootBone == bone.GetName(); });
		_UpdateAnimation(*(root), anim->GetFrameByNumber(_frame), UNIT_MATRIX); //@ potentially not thread safe
		for (auto& bone : bones)
		{
			ret[bone.GetID()] = bone.getAnimatedTransform();
		}
		return ret;
	}
}

//------------------------------------------------------------------
size_t Rigger::GetAnimationCount() const
{
	return animations.size();
}

//-------------------------------------------------------------------------------------------
size_t Rigger::GetBoneCount() const
{
	return bones.size();
}

//-------------------------------------------------------------
const std::string& Rigger::GetCurrentAnimationName() const
{
	if (currentAnim != nullptr)
		return currentAnim->Name();
	else
		throw("logic error");
}

//-------------------------------------------------------------------------------------------
size_t Rigger::GetCurrentAnimationFrameIndex() const
{
	if (currentAnim != nullptr)
		return currentAnim->GetCurFrameIndex();
	else
		return -1;
}

//-------------------------------------------------------------------------------------------
void Rigger::AddAnimations(std::vector<SceletalAnimation> _animations)
{
	animations.insert(animations.end(),
										std::make_move_iterator(_animations.begin()),
										std::make_move_iterator(_animations.end()));
}

//-------------------------------------------------------------------------------------------
std::vector<std::string> Rigger::GetAnimationNames() const
{
	std::vector<std::string> names;
	for (auto& anim : animations)
		names.push_back(anim.Name());
	return names;
}

//-------------------------------------------------------------------------------------------
std::vector<std::string> Rigger::GetBoneNames() const
{
	std::vector<std::string> names;
	for (auto& bone : bones)
		names.push_back(bone.GetName());
	return names;
}

//-------------------------------------------------------------------------------------------
glm::mat4 Rigger::GetBindMatrixForBone(const std::string& _boneName) const
{
	auto it = std::find_if(bones.begin(), bones.end(), [_boneName](const Bone& bone) {return bone.GetName() == _boneName; });
	if (it != bones.end())
		return it->GetLocalBindTransform();
	return glm::mat4();
}

//-------------------------------------------------------------------------------------------
glm::mat4 Rigger::GetBindMatrixForBone(size_t _boneID) const
{
	if (_boneID < bones.size())
		return GetBindMatrixForBone(bones[_boneID].GetName());
	return glm::mat4();
}

//-------------------------------------------------------------------------------------------
glm::mat4 Rigger::GetCurrentMatrixForBone(const std::string& _boneName) const
{
	auto it = std::find_if(bones.begin(), bones.end(), [_boneName](const Bone& bone) {return bone.GetName() == _boneName; });
	if (it != bones.end())
		return it->getAnimatedTransform();
	return glm::mat4();
}

//-------------------------------------------------------------------------------------------
glm::mat4 Rigger::GetCurrentMatrixForBone(size_t _boneID) const
{
	if (_boneID < bones.size())
		return GetCurrentMatrixForBone(bones[_boneID].GetName());
	return glm::mat4();
}

//-------------------------------------------------------------------------------------------
void Rigger::_Initialize()
{
	matrices.resize(bones.size());
	for (auto& m : matrices)
		m = UNIT_MATRIX;

	//adding copied bones as children and deleting old connections to origin bones
	for (auto& bone : bones)
	{
		std::vector<Bone*> children = bone.getChildren();
		for (Bone* child : children)
		{
			auto iter = std::find_if(bones.begin(), bones.end(), [&child](const Bone& bone) {return child->GetName() == bone.GetName(); });
			bone.addChild(&(*iter));
		}
		for (Bone*& old_child : children)
		{
			std::vector<Bone*>& new_children = bone.getChildren();
			new_children.erase(std::remove_if(new_children.begin(), new_children.end(), [&old_child](Bone* _bone){ return _bone == old_child; }));
		}
	}

	timer.reset(new math::Timer([this]()->bool
		{
			if (currentAnim != nullptr && !currentAnim->IsPaused())
			{
				bool fls = false;
				if (matrix_flag.compare_exchange_weak(fls, true))
				{
					auto root = std::find_if(bones.begin(), bones.end(), [this](const Bone& bone) { return nameRootBone == bone.GetName(); });
					_UpdateAnimation(*(root), currentAnim->getCurrentFrame(), UNIT_MATRIX);
					for (auto& bone : bones)
					{
						matrices[bone.GetID()] = bone.getAnimatedTransform();
					}
					matrix_flag.store(false);
					std::this_thread::yield();
				}
			}
			else if (is_active) // currentAnim == nullptr || currentAnim->IsPaused()
			{
				std::unique_lock lk(mutex);
				cv.wait(lk);
			}
			return true;
		}));
	timer->start(100);
}

//-------------------------------------------------------------------------------------------
void Rigger::_UpdateAnimation(Bone &bone, const Frame& frame, const glm::mat4 &ParentTransform)
{
	glm::mat4 currentLocalTransform;
	
	if(frame.exists(bone.GetName()))
		currentLocalTransform = frame.pose.find(bone.GetName())->second.getModelMatrix();
	else
		currentLocalTransform = bone.GetMTransform();
	
	glm::mat4 globalTransform = ParentTransform * currentLocalTransform;

	glm::mat4 totalTransform = globalModelTransform * globalTransform * bone.GetLocalBindTransform(); // OGLDev
	bone.setAnimatedTransform(totalTransform);

	for(int i = 0; i<bone.NumChildren(); ++i)
		_UpdateAnimation(*(bone.getChildren()[i]), frame, globalTransform);
}

#include "stdafx.h"
#include "Rigger.h"
#include "AnimatedModel.h"

#include <algorithm>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iterator>

//-------------------------------------------------------------------------------------------
Rigger::Rigger(eAnimatedModel* _model):model(_model)
{
	if(model)
	{
		animations		= model->Animations();
		bones			= model->Bones();
		nameRootBone	= model->RootBoneName();

		matrices.resize(bones.size());
		for(auto& m : matrices)
		{
			m = UNIT_MATRIX;
		}
		for(auto& bone: bones)
		{
			auto children = bone.getChildren();
			for(auto& child : children)
			{
				auto iter = std::find_if(bones.begin(), bones.end(), [&child](const Bone& bone ) {return child->Name() == bone.Name();});
				bone.addChild(&(*iter));	//$todo remove old children
			}
		}

		timer.reset(new math::Timer([this]()->bool
									{
										if (currentAnim != nullptr && !currentAnim->IsPaused())
										{
											bool fls = false;
											if (matrix_flag.compare_exchange_weak(fls, true))
											{
												auto root = std::find_if(bones.begin(), bones.end(), [this](const Bone& bone) { return nameRootBone == bone.Name(); });
												UpdateAnimation(*(root), currentAnim->getCurrentFrame(), UNIT_MATRIX);
												for (auto& bone : bones)
												{
													matrices[bone.ID()] = bone.getAnimatedTransform();
												}
												matrix_flag.store(false);
												std::this_thread::yield();
											}
										}
										else if(is_active) // currentAnim == nullptr || currentAnim->IsPaused()
										{
											std::unique_lock lk(mutex);
											cv.wait(lk);
										}
										return true;
									}));
		timer->start(100);
	}
}
//-------------------------------------------------------------------------------------------
Rigger::~Rigger()
{
	is_active = false; // no more sleep
	cv.notify_one(); // wake up
}

//-------------------------------------------------------------------------------------------
bool Rigger::Apply(const std::string & _animation, bool _play_once)
{
	if(currentAnim != nullptr && currentAnim->Name() == _animation )
	{
		currentAnim->Continue();
		return true;
	}
	else
	{
		auto anim = std::find_if(animations.begin(), animations.end(),
			[_animation](const SceletalAnimation& anim) { return anim.Name() == _animation; });
		if(anim != animations.end())
		{
			currentAnim = &(*anim);
			if (_play_once)
				currentAnim->PlayOnce();
			else
				currentAnim->Start();
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
		names.push_back(bone.Name());
	return names;
}

//-------------------------------------------------------------------------------------------
glm::mat4 Rigger::GetBindMatrixForBone(const std::string& _boneName) const
{
	auto it = std::find_if(bones.begin(), bones.end(), [_boneName](const Bone& bone) {return bone.Name() == _boneName; });
	if (it != bones.end())
		return it->getBindTransform();
	return glm::mat4();
}

//-------------------------------------------------------------------------------------------
glm::mat4 Rigger::GetBindMatrixForBone(size_t _boneID) const
{
	if (_boneID < bones.size())
		return GetBindMatrixForBone(bones[_boneID].Name());
	return glm::mat4();
}

//-------------------------------------------------------------------------------------------
glm::mat4 Rigger::GetCurrentMatrixForBone(const std::string& _boneName) const
{
	auto it = std::find_if(bones.begin(), bones.end(), [_boneName](const Bone& bone) {return bone.Name() == _boneName; });
	if (it != bones.end())
		return it->getAnimatedTransform();
	return glm::mat4();
}

//-------------------------------------------------------------------------------------------
glm::mat4 Rigger::GetCurrentMatrixForBone(size_t _boneID) const
{
	if (_boneID < bones.size())
		return GetCurrentMatrixForBone(bones[_boneID].Name());
	return glm::mat4();
}

//-------------------------------------------------------------------------------------------
void Rigger::UpdateAnimation(Bone &bone, const Frame& frame, const glm::mat4 &ParentTransform)
{
	glm::mat4 currentLocalTransform;
	
	if(frame.exists(bone.Name()))
		currentLocalTransform = frame.pose.find(bone.Name())->second.getModelMatrix();
	else
		currentLocalTransform = bone.getMTransform();
	
	glm::mat4 globalTransform = ParentTransform * currentLocalTransform;

	glm::mat4 totalTransform = model->GlobalTransform() * globalTransform * bone.getBindTransform(); // OGLDev
	bone.setAnimatedTransform(totalTransform);

	for(int i = 0; i<bone.NumChildren(); ++i)
		UpdateAnimation(*(bone.getChildren()[i]), frame, globalTransform);
}

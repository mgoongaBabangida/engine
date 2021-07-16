#include "stdafx.h"
#include "Rigger.h"
#include "AnimatedModel.h"

#include <algorithm>
#include <iostream>
#include <thread>

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
										bool fls = false;
										if(currentAnim != nullptr && !currentAnim->IsPaused() && matrix_flag.compare_exchange_weak(fls, true))
										{
											auto root = std::find_if(bones.begin(), bones.end(), [this](const Bone& bone) { return nameRootBone == bone.Name(); });
											UpdateAnimation(*(root), currentAnim->getCurrentFrame(), UNIT_MATRIX);
											for(auto& bone : bones)
											{
												matrices[bone.ID()] = bone.getAnimatedTransform();
											}
											matrix_flag.store(false);
											std::this_thread::yield();
										}
										return true;
									}));
		timer->start(100);
	}
}

//-------------------------------------------------------------------------------------------
bool Rigger::Apply(const std::string & _animation)
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
bool Rigger::Apply(size_t _animation_index)
{
  if (currentAnim != nullptr && currentAnim == &animations[_animation_index])
  {
    currentAnim->Continue();
    return true;
  }
  else if(_animation_index < animations.size())
  {
    currentAnim = &animations[_animation_index];
    currentAnim->Start();
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

#include "stdafx.h"
#include "Rigger.h"
#include "AssimpModel.h"

Rigger::Rigger(Model * _model):model(_model)
{
	if (model)
	{
		animations		= model->Animations();
		bones			= model->Bones();
		nameRootBone	= model->RootBoneName();

		matrices.resize(bones.size());
		for (auto& bone:bones)
		{
			auto children = bone.getChildren();
			for (auto& child : children)
			{
				auto iter = std::find_if(bones.begin(), bones.end(), [&child](const Bone& bone ) {return child->Name() == bone.Name();});
				bone.addChild(&(*iter));
				//TODO remove old children
			}
		}
	}
}

bool Rigger::Apply(const std::string & _animation)
{
	if (currentAnim != nullptr && currentAnim->Name() == _animation )
	{
		currentAnim->Continue();
		return true;
	}
	else
	{
		auto anim = std::find_if(animations.begin(), animations.end(),
			[_animation](const SceletalAnimation& anim) { return anim.Name() == _animation; });
		if (anim != animations.end())
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

void Rigger::Stop()
{
	currentAnim->Stop();
}

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

const std::vector<glm::mat4>& Rigger::GetMatrices()
{
	if(currentAnim != nullptr && !currentAnim->IsPaused())
	{
		auto root = std::find_if(bones.begin(), bones.end(), [this](const Bone& bone) { return nameRootBone == bone.Name(); });
		UpdateAnimation(*(root), currentAnim->getCurrentFrame(), glm::mat4()); 
		for (auto& bone : bones) {
			matrices[bone.ID()] = bone.getAnimatedTransform();
		}
	}
	return matrices;
}

void Rigger::UpdateAnimation(Bone &bone, const Frame& frame, const glm::mat4 &ParentTransform)
{
	glm::mat4 currentLocalTransform;
	
	if (frame.exists(bone.Name()))
		currentLocalTransform = frame.pose.find(bone.Name())->second.getModelMatrix();
	else
		currentLocalTransform = bone.getMTransform();
	
	glm::mat4 globalTransform = ParentTransform * currentLocalTransform;

	//glm::mat4 totalTransform = currentTransform * bone.getInverseBindTransform(); //ThinMatrix
	glm::mat4 totalTransform = model->GlobalTransform() * globalTransform * bone.getBindTransform(); // OGLDev
	/*std::cout << "--------------------------------------------------------------" << std::endl;
	std::cout << totalTransform[0][1] << " " << totalTransform[0][2] << " " << totalTransform[0][3] << std::endl;
	std::cout << totalTransform[1][1] << " " << totalTransform[1][2] << " " << totalTransform[1][3] << std::endl;
	std::cout << totalTransform[2][1] << " " << totalTransform[2][2] << " " << totalTransform[2][3] << std::endl;
	std::cout << "--------------------------------------------------------------" << std::endl;*/
	bone.setAnimatedTransform(totalTransform);

	for (int i = 0; i<bone.NumChildren(); ++i)
		UpdateAnimation(*(bone.getChildren()[i]), frame, globalTransform);
}

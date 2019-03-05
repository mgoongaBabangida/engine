#pragma once
#include "Bone.h"
#include "SceletalAnimation.h"
#include <string>

class Model;

class Rigger
{
public:
	Rigger(Model* _model);
	bool							Apply(const std::string& _animation);
	void							Stop();
	bool							ChangeName(const std::string& _oldName, const std::string& _newName);
	const std::vector<glm::mat4>&	GetMatrices();

protected:
	void							UpdateAnimation(Bone & bone, const Frame & frame, const glm::mat4 & ParentTransform);

	/*const*/ Model*				model;
	std::vector<SceletalAnimation>	animations;
	SceletalAnimation*				currentAnim = nullptr;
	std::vector<Bone>				bones;
	std::string						nameRootBone;
	std::vector<glm::mat4>			matrices;
};
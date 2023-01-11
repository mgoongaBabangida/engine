#pragma once

#include "stdafx.h"
#include <base/interfaces.h>

#include "Bone.h"
#include "SceletalAnimation.h"

//------------------------------------------------
class DLL_MATH eAnimatedModel : public IModel
{
public:
	virtual std::string						RootBoneName() = 0;
	virtual std::vector<Bone>				Bones()	const = 0;
	virtual std::vector<SceletalAnimation>	Animations()const = 0;
	virtual glm::mat4						GlobalTransform() const = 0;
};

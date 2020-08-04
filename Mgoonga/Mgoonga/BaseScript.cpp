#include "stdafx.h"
#include "BaseScript.h"
#include <base/Object.h>
#include <math/Transform.h>
#include <math/BoxCollider.h>

//---------------------------------------------------------------------------
eBaseScript::eBaseScript(Texture* flag_texture)
	: flag_tex(flag_texture)
{

}

Flag eBaseScript::GetFlag(const Camera & camera)
{
	//getting top 4 corners of the model
	glm::vec4 top_corners[4];
	top_corners[0] = object->GetTransform()->getModelMatrix() * glm::vec4(object->GetCollider()->getMaxX(), object->GetCollider()->getMaxY(), object->GetCollider()->getMaxZ(), 1.0f);
	top_corners[1] = object->GetTransform()->getModelMatrix() * glm::vec4(object->GetCollider()->getMaxX(), object->GetCollider()->getMaxY(), object->GetCollider()->getMinZ(), 1.0f);
	top_corners[2] = object->GetTransform()->getModelMatrix() * glm::vec4(object->GetCollider()->getMinX(), object->GetCollider()->getMaxY(), object->GetCollider()->getMaxZ(), 1.0f);
	top_corners[3] = object->GetTransform()->getModelMatrix() * glm::vec4(object->GetCollider()->getMinX(), object->GetCollider()->getMaxY(), object->GetCollider()->getMinZ(), 1.0f);
	glm::vec4 position = top_corners[0];
	//Choosing the corner in relation to camera position
	for (int i = 0; i < 4; ++i)
	{
		position = glm::length2(camera.getPosition() - glm::vec3(top_corners[i]))
			< glm::length2(camera.getPosition() - glm::vec3(position)) ? top_corners[i] : position;
	}
	return Flag(position, flag_tex, flag_scale);
}

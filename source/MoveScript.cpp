#include "MoveScript.h"
#include "Structures.h"
#include "ParticleRender.h"
#include "ShootingParticleSystem.h"
#include "Sound.h"

MoveScript::MoveScript(Texture* flag_texture, eParticleRender* prt, Texture* shoting_texture, remSnd* shooting_sound)
	:flag_tex(flag_texture), prt_renderer(prt),shoot_tex(shoting_texture),shoot_snd(shooting_sound)
{
	turn_speed = str::PI / 4.0f / 60.0f;
	move_speed = 1.0f / 120.0f;
}

void MoveScript::Update(std::vector<std::shared_ptr<eObject> > objs)
{
	if (object != nullptr && destination != glm::vec3(-100.0f, -100.0f, -100.0f) )
	{
		if (!object->getTransform()->turnTo(destination, turn_speed)
			&& glm::length2(object->getTransform()->getTranslation() - destination) > 0.1f) //change 0.1f
			{	
				object->MoveForward(objs); // transfer to transform and transfer move speed
				//std::cout << "length2= "<<glm::length2(object->getTransform()->getTranslation() - destination)<<std::endl;
			}
	}
}

void MoveScript::setDestination(glm::vec3 dst) { destination = dst; }


Flag* MoveScript::getFlag(const Camera& camera)
{
	//getting top 4 corners of the model
	glm::vec4 top_corners[4];
	top_corners[0] = object->getTransform()->getModelMatrix() * glm::vec4(object->getCollider()->getMaxX(), object->getCollider()->getMaxY(), object->getCollider()->getMaxZ(), 1.0f) ;
	top_corners[1] = object->getTransform()->getModelMatrix() * glm::vec4(object->getCollider()->getMaxX(), object->getCollider()->getMaxY(), object->getCollider()->getMinZ(), 1.0f) ;
	top_corners[2] = object->getTransform()->getModelMatrix() * glm::vec4(object->getCollider()->getMinX(), object->getCollider()->getMaxY(), object->getCollider()->getMaxZ(), 1.0f) ;
	top_corners[3] = object->getTransform()->getModelMatrix() * glm::vec4(object->getCollider()->getMinX(), object->getCollider()->getMaxY(), object->getCollider()->getMinZ(), 1.0f) ;
	glm::vec4 position = top_corners[0];
	//Choosing the corner in relation to camera position
	for (int i = 0; i < 4; ++i)
	{
		position = glm::length2(camera.getPosition() - glm::vec3(top_corners[i])) < glm::length2(camera.getPosition() - glm::vec3(position)) ? top_corners[i] : position;
	}
	return new Flag(position,flag_tex);
}

void MoveScript::shoot()
{
	ParticleSystemInfo info;
	info.texture			= shoot_tex;
	info.scale				= 0.05;
	
	glm::vec4 modelCenter	= object->getTransform()->getModelMatrix() * glm::vec4(object->getCollider()->getCenter(), 1.0f);//glm::vec3(0.0f, 4.0f, -0.5f);
	info.systemCenter		= modelCenter + object->getTransform()->getRotationVector();
	IParticleSystem* system = new ShootingParticleSystem(10, 0, 0, 10000, new remSnd(*shoot_snd),10000); //TODO: check leaks
	prt_renderer->AddParticleSystem(system, info);
}
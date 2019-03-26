#include "stdafx.h"
#include "ShipScript.h"
#include "Structures.h"
#include "ParticleRender.h"
#include "ShootingParticleSystem.h"
#include "Sound.h"

eShipScript::eShipScript(Texture* flag_texture, eParticleRender* prt, Texture* shoting_texture, remSnd* shooting_sound, dbb::CameraRay* _camRay, float _waterHeight)
	:flag_tex(flag_texture), prt_renderer(prt),shoot_tex(shoting_texture),shoot_snd(shooting_sound), camRay(_camRay), waterHeight(_waterHeight)
{
	turn_speed = PI / 4.0f / 60.0f;
	move_speed = 1.0f / 120.0f;
}

bool eShipScript::OnKeyPress(uint32_t asci)
{
	if(asci == ASCII_G)
	{
		Shoot();
		return true;
	}
	return false;
}

bool eShipScript::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	if(!left)
	{
		dbb::plane pl(glm::vec3(1.0f, waterHeight, 1.0f),
					  glm::vec3(0.0f, waterHeight, 0.0f),
					  glm::vec3(0.0f, waterHeight, 1.0f)); // arbitrary triangle on waterHeight plane
		glm::vec3 target = dbb::intersection(pl, camRay->getLine());
		SetDestination(target);
	}
	return true;
}

void eShipScript::Update(std::vector<std::shared_ptr<eObject> > objs)
{
	std::vector<std::shared_ptr<eObject> > objsToCollide;
	for (std::shared_ptr<eObject> obj : objs)
	{
		if(obj->Name() != "Terrain")
		{
			objsToCollide.push_back(obj);
		}
	}

	if(object && destination != NONE)
	{
		if(!object->getTransform()->turnTo(destination, turn_speed)
			&& glm::length2(object->getTransform()->getTranslation() - destination) > 0.1f) //$todo 0.1f
			{	
				object->MoveForward(objsToCollide);
			}
	}
}

Flag eShipScript::GetFlag(const Camera& camera)
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
		position = glm::length2(camera.getPosition() - glm::vec3(top_corners[i])) 
				< glm::length2(camera.getPosition() - glm::vec3(position)) ? top_corners[i] : position;
	}
	return Flag(position, flag_tex);
}

void eShipScript::Shoot()
{
	glm::vec4 modelCenter	=	object->getTransform()->getModelMatrix() 
								* glm::vec4(object->getCollider()->getCenter(), 1.0f);
	IParticleSystem* system = new ShootingParticleSystem(10, 0, 0, 10000, 
														modelCenter + object->getTransform()->getRotationVector(), 
														shoot_tex, 
														shoot_snd,
														10000);
	prt_renderer->AddParticleSystem(system);
}
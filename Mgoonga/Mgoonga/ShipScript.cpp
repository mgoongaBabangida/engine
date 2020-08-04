#include "stdafx.h"
#include "ShipScript.h"
#include <base/base.h>
#include <opengl_assets\RenderManager.h>
#include <math/ShootingParticleSystem.h>
#include <opengl_assets\Sound.h>
#include <math/BoxCollider.h>
#include <math/RigidBdy.h>

//----------------------------------------------------------------
eShipScript::eShipScript(Texture*			_flagTexture,
						 eRenderManager&	_render_manager, 
						 Texture*			_shoting_texture, 
						 RemSnd*			_shooting_sound, 
						 dbb::CameraRay*	_camRay, 
						 float				_waterHeight)
: eBaseScript(_flagTexture)
, render_manager(_render_manager)
, shoot_tex(_shoting_texture)
, shoot_snd(_shooting_sound)
, camRay(_camRay)
, waterHeight(_waterHeight)
{
	turn_speed = PI / 4.0f / 60.0f;
	move_speed = 1.0f / 240.0f;
	flag_scale = { 0.03f, 0.03f ,0.03f };
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

//------------------------------------------------------------------------------------
bool eShipScript::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	//if(!left)
	//{
	//	dbb::plane pl(glm::vec3(1.0f, waterHeight, 1.0f),
	//				  glm::vec3(0.0f, waterHeight, 0.0f),
	//				  glm::vec3(0.0f, waterHeight, 1.0f)); // arbitrary triangle on waterHeight plane
	//	glm::vec3 target = dbb::intersection(pl, camRay->getLine());

	//	if (hexes)
	//	{
	//		for (auto& hex : *hexes)
	//		{
	//			glm::vec4 hex_transformed = inverse_transform_terrain * glm::vec4{ hex.x(), waterHeight, hex.z(), 1.0f };
	//			if (hex.IsOn(target.x, target.z)
	//				&& terrain->GetHeight(hex_transformed.x, hex_transformed.z) < hex_transformed.y)
	//			{
	//				SetDestination(glm::vec3{ hex.x(), waterHeight, hex.z() });
	//				return true;
	//			}
	//		}
	//	}
	//}
	return false;
}

//-------------------------------------------------------------------------
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
		if (!object->GetTransform()->turnTo(destination, turn_speed))
		{
			if (glm::length2(object->GetTransform()->getTranslation() - destination) > move_speed)
			{
				object->GetRigidBody()->MoveForward(objsToCollide);
			}
			else
			{
				object->GetTransform()->setTranslation(destination);
				destination = NONE;
				if (shoot_after_move)
					Shoot();
			}
		}
	}
}

//----------------------------------------------------------------------------------
void eShipScript::Shoot()
{
	glm::vec4 modelCenter	=	object->GetTransform()->getModelMatrix() 
								* glm::vec4(object->GetCollider()->GetCenter(), 1.0f);
	IParticleSystem* system = new ShootingParticleSystem(10, 0, 0, 10000, 
														modelCenter + object->GetTransform()->getRotationVector() * 0.2f, // ask hexes
														shoot_tex, 
														shoot_snd,
														10000);
	render_manager.get().AddParticleSystem(system);
	shoot_after_move = false;
}
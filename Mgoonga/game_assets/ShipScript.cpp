#include "stdafx.h"
#include "ShipScript.h"
#include <base/base.h>
#include <opengl_assets/openglrenderpipeline.h>
#include <math/ShootingParticleSystem.h>
#include <opengl_assets\Sound.h>
#include <opengl_assets\MyModel.h>
#include <math/BoxCollider.h>
#include <math/RigidBdy.h>

//----------------------------------------------------------------
eShipScript::eShipScript(Texture*			_flagTexture,
						 eOpenGlRenderPipeline&		_pipeline,
	           Camera&									_camera,
						 Texture*									_shoting_texture,
						 RemSnd*									_shooting_sound,
						 float										_waterHeight)
: pipeline(_pipeline)
, camera(_camera)
, shoot_tex(_shoting_texture)
, shoot_snd(_shooting_sound)
, waterHeight(_waterHeight)
, flag_tex(_flagTexture)
, flag(new eObject{})
{
	turn_speed = PI / 4.0f / 60.0f;
	move_speed = 1.0f / 240.0f;
	flag_scale = { 0.03f, 0.03f ,0.03f };
	flag->SetTransform(new Transform);
	std::shared_ptr<MyMesh> mesh(new MyMesh());
	flag->SetModel(new MyModel(mesh, flag_tex));
}

//-------------------------------------------------------------------
eShipScript::~eShipScript()
{
}

//------------------------------------------------------------------------------------
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
std::vector<shObject> eShipScript::GetChildrenObjects()
{
  //getting top 4 corners of the model
  glm::vec4 top_corners[4]; //@todo improve
  top_corners[0] = object->GetTransform()->getModelMatrix() * glm::vec4(object->GetCollider()->GetExtremDotsLocalSpace().MaxX, object->GetCollider()->GetExtremDotsLocalSpace().MaxY, object->GetCollider()->GetExtremDotsLocalSpace().MaxZ, 1.0f);
	top_corners[1] = object->GetTransform()->getModelMatrix() * glm::vec4(object->GetCollider()->GetExtremDotsLocalSpace().MaxX, object->GetCollider()->GetExtremDotsLocalSpace().MaxY, object->GetCollider()->GetExtremDotsLocalSpace().MaxZ, 1.0f);
	top_corners[2] = object->GetTransform()->getModelMatrix() * glm::vec4(object->GetCollider()->GetExtremDotsLocalSpace().MinX, object->GetCollider()->GetExtremDotsLocalSpace().MinY, object->GetCollider()->GetExtremDotsLocalSpace().MinZ, 1.0f);
	top_corners[3] = object->GetTransform()->getModelMatrix() * glm::vec4(object->GetCollider()->GetExtremDotsLocalSpace().MinX, object->GetCollider()->GetExtremDotsLocalSpace().MinY, object->GetCollider()->GetExtremDotsLocalSpace().MinZ, 1.0f);
  glm::vec4 position = top_corners[0];
  //Choosing the corner in relation to camera position
  for (int i = 0; i < 4; ++i)
  {
		position = glm::length2(camera.get().getPosition() - glm::vec3(top_corners[i]))
			< glm::length2(camera.get().getPosition() - glm::vec3(position)) ? top_corners[i] : position;
  }

  flag->GetTransform()->setTranslation(position);
  flag->GetTransform()->setScale(flag_scale);
  return { flag };
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
	//@todo no connection to rendering. Add pt sys in different way
	pipeline.get().AddParticleSystem(system);
	shoot_after_move = false;
}

#include "stdafx.h"
#include "ShipScript.h"
#include <base/base.h>

#include <math/ParticleSystem.h>
#include <math/BoxCollider.h>
#include <math/RigidBdy.h>

#include <opengl_assets/Sound.h>
#include <opengl_assets/MyModel.h>
#include <opengl_assets/openglrenderpipeline.h>

//----------------------------------------------------------------
eShipScript::eShipScript(IGame* _game,
												const Texture*			_flagTexture,
												eOpenGlRenderPipeline&		_pipeline,
												Camera&									_camera,
												const Texture*						_shoting_texture,
												RemSnd*									_shooting_sound,
												float										_waterHeight)
: m_game(_game)
, pipeline(_pipeline)
, camera(_camera)
, shoot_tex(_shoting_texture)
, shoot_snd(_shooting_sound)
, waterHeight(_waterHeight)
, flag_tex(_flagTexture)
, m_flag(new eObject{})
{
	turn_speed = PI / 4.0f / 60.0f;
	move_speed = 1.0f / 240.0f;
	flag_scale = { 0.02f, 0.02f ,0.02f };

	m_flag->SetTransform(new Transform);
	std::shared_ptr<MyMesh> mesh(new MyMesh("flag"));
	m_flag->SetModel(new MyModel(mesh,"flag_mesh", flag_tex));
	m_flag->SetRenderType(eObject::RenderType::FLAG);
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
bool eShipScript::OnMousePress(int32_t x, int32_t y, bool left)
{
	if(m_game->GetFocusedObject().get() == this->object)
	{
		//if (!left)
		//{
		//	dbb::plane pl(glm::vec3(1.0f, waterHeight, 1.0f),
		//		glm::vec3(0.0f, waterHeight, 0.0f),
		//		glm::vec3(0.0f, waterHeight, 1.0f)); // arbitrary triangle on waterHeight plane
		//	glm::vec3 target = dbb::intersection(pl, camera.get().getCameraRay().getLine());
		//	SetDestination(target);
		//}

	/*	if (hexes)
		{
			for (auto& hex : *hexes)
			{
				glm::vec4 hex_transformed = inverse_transform_terrain * glm::vec4{ hex.x(), waterHeight, hex.z(), 1.0f };
				if (hex.IsOn(target.x, target.z)
					&& terrain->GetHeight(hex_transformed.x, hex_transformed.z) < hex_transformed.y)
				{
					SetDestination(glm::vec3{ hex.x(), waterHeight, hex.z() });
					return true;
				}
			}
		}*/
	}
	return false;
}

//-------------------------------------------------------------------------
void eShipScript::Update(float _tick)
{
	_UpdateFlagPos();

	auto objs = m_game->GetObjects();
	std::vector<std::shared_ptr<eObject> > objsToCollide;
	for (std::shared_ptr<eObject> obj : objs)
	{
		if(obj->Name() != "Terrain")
			objsToCollide.push_back(obj);
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
	std::shared_ptr<IParticleSystem> system = std::make_shared<ParticleSystem>(10, 0, 0, 10000,
														modelCenter + object->GetTransform()->getRotationVector() * 0.2f, // ask hexes
														shoot_tex,
														shoot_snd,
														10000);
	//@todo no connection to rendering. Add pt sys in different way
	system->Start();
	pipeline.get().AddParticleSystem(system);
	shoot_after_move = false;
}

//----------------------------------------------------------------------------------
void eShipScript::_UpdateFlagPos()
{
	if (object->GetChildrenObjects().empty())
		object->GetChildrenObjects().push_back(m_flag);

	std::shared_ptr<eObject>	flag = object->GetChildrenObjects()[0];

		//getting top 4 corners of the model
		glm::vec4 top_corners[4]; //@todo improve
		top_corners[0] = object->GetTransform()->getModelMatrix() * glm::vec4(object->GetCollider()->GetExtremDotsLocalSpace().MaxX, object->GetCollider()->GetExtremDotsLocalSpace().MaxY, object->GetCollider()->GetExtremDotsLocalSpace().MaxZ, 1.0f);
		top_corners[1] = object->GetTransform()->getModelMatrix() * glm::vec4(object->GetCollider()->GetExtremDotsLocalSpace().MaxX, object->GetCollider()->GetExtremDotsLocalSpace().MaxY, object->GetCollider()->GetExtremDotsLocalSpace().MinZ, 1.0f);
		top_corners[2] = object->GetTransform()->getModelMatrix() * glm::vec4(object->GetCollider()->GetExtremDotsLocalSpace().MinX, object->GetCollider()->GetExtremDotsLocalSpace().MaxY, object->GetCollider()->GetExtremDotsLocalSpace().MaxZ, 1.0f);
		top_corners[3] = object->GetTransform()->getModelMatrix() * glm::vec4(object->GetCollider()->GetExtremDotsLocalSpace().MinX, object->GetCollider()->GetExtremDotsLocalSpace().MaxY, object->GetCollider()->GetExtremDotsLocalSpace().MinZ, 1.0f);
		glm::vec4 position = top_corners[0];
		//Choosing the corner in relation to camera position
		for (int i = 0; i < 4; ++i)
		{
			float new_length2 = glm::length2(camera.get().getPosition() - glm::vec3(top_corners[i]));
			float cur_length2 = glm::length2(camera.get().getPosition() - glm::vec3(position));
			position = new_length2 < cur_length2 ? top_corners[i] : position;
		}

		flag->GetTransform()->setTranslation(position);
		flag->GetTransform()->setScale(flag_scale);
}

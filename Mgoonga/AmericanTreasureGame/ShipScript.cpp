
#include "ShipScript.h"
#include <base/base.h>

#include <math/ParticleSystem.h>
#include <math/BoxCollider.h>
#include <math/RigidBody.h>

#include <opengl_assets/Sound.h>
#include <opengl_assets/MyModel.h>

#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>

//----------------------------------------------------------------
eShipScript::eShipScript(IGame* _game,
												const Texture*					_flagTexture,
												Camera&									_camera,
												const Texture*					_shoting_texture,
												RemSnd*									_shooting_sound,
												float										_waterHeight)
: m_game(_game)
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
	m_flag->SetModel(new MyModel(mesh,"flag_mesh", flag_tex, flag_tex));
	m_flag->SetRenderType(eObject::RenderType::FLAG);
}

//-------------------------------------------------------------------
eShipScript::~eShipScript()
{
	m_game->DeleteInputObserver(this);
}

//------------------------------------------------------------------------------------
bool eShipScript::OnMousePress(int32_t x, int32_t y, bool left, KeyModifiers _modifier)
{
	if (shObject object = m_object.lock(); object)
	{
		if (m_game->GetFocusedObject().get() == object.get())
		{
		}
	}
	return false;
}

//-------------------------------------------------------------------------
void eShipScript::CollisionCallback(const eCollision& _collision)
{
	if (shObject object = m_object.lock(); object)
	{
		if (_collision.collider == object.get() && destination == NONE)
		{
			auto objs = m_game->GetObjects();
			std::vector<std::shared_ptr<eObject> > objsToCollide;
			for (std::shared_ptr<eObject> obj : objs)
			{
				if (obj->Name() != "Terrain")
					objsToCollide.push_back(obj);
			}

			//@todo test Upvector and rotationupvector (-5?)
			static float collision_rotation = 0.0f;
			if (collision_rotation == 360.0f)
			{
				collision_rotation = 0.0f;
				return;
			}
			auto rot = glm::toQuat(glm::rotate(UNIT_MATRIX, glm::radians(5.0f)/*(-5 ? )*/, glm::vec3(0.0f, 1.0f, 0.0f)));
			collision_rotation += 5.0f;
			object->GetTransform()->setRotation(rot * object->GetTransform()->getRotation());
			object->GetRigidBody()->Move(objsToCollide);
		}
	}
}

//-------------------------------------------------------------------------
void eShipScript::Update(float _tick)
{
	_UpdateFlagPos();
	if (shObject object = m_object.lock(); object)
	{
		if (object && destination != NONE)
		{
			auto objs = m_game->GetObjects();
			std::vector<std::shared_ptr<eObject> > objsToCollide;
			for (std::shared_ptr<eObject> obj : objs)
			{
				if (obj->Name() != "Terrain")
					objsToCollide.push_back(obj);
			}

			if (!object->GetTransform()->turnTo(destination, turn_speed))
			{
				if (glm::length2(object->GetTransform()->getTranslation() - destination) > move_speed)
				{
					object->GetRigidBody()->MoveForward({});
				}
				else
				{ //@todo if destionation is not final there might be bugs, test
					object->GetTransform()->setTranslation(destination);
					destination = NONE;
					object->GetRigidBody()->Move(objsToCollide);
				}
			}
		}
		else if (object && m_drowned && !m_drowning_animation && m_state == ALIVE)
		{
			m_state = HIT;
			m_drowning_animation = std::make_unique<math::AnimationLeaner<float>>(std::vector<float>{0}, std::vector<float>{PI / 4}, 5'000);
			m_drowning_animation->Start();
			m_drawn_rotation = object->GetTransform()->getRotation();
		}
		else if (object && m_drowning_animation && m_state == HIT && m_drowning_animation->IsOn())
		{
			float t = m_drowning_animation->getCurrentFrame()[0];
			auto rot = glm::toQuat(glm::rotate(UNIT_MATRIX, t, -glm::vec3(object->GetTransform()->getRotationVector())));
			object->GetTransform()->setRotation(rot * m_drawn_rotation);
		}
		else if (object && m_drowned && m_state == HIT)
		{
			m_state = DROWNING;
			m_drowning_animation = std::make_unique<math::AnimationLeaner<float>>(std::vector<float>{ object->GetTransform()->getTranslation().y, },
				std::vector<float>{ object->GetTransform()->getTranslation().y - 0.2f},
				2'000);
			m_drowning_animation->Start();
		}
		else if (object && m_drowned && m_state == DROWNING && m_drowning_animation->IsOn())
		{
			glm::vec3 translation = object->GetTransform()->getTranslation();
			object->GetTransform()->setTranslation({ translation.x, m_drowning_animation->getCurrentFrame()[0], translation.z });
		}
		else if (object && m_drowned && m_state == DROWNING)
		{
			m_state = DROWNED;
			auto objects = m_game->GetObjects();
			for (int i = 0; i < objects.size(); ++i)
			{
				if (objects[i]->GetScript() == this)
				{
					objects[i]->SetVisible(false);
					m_game->DeleteObject(objects[i]);
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------
void eShipScript::_UpdateFlagPos()
{
	if (shObject object = m_object.lock(); object)
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
}

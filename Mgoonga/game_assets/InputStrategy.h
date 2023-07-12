#pragma once
#include <base/interfaces.h>
#include <math/PlaneLine.h>
#include <math/Camera.h>

//-------------------------------------------------------------------------------
class InputStrategy : public IInputObserver
{
public:
	//IInputObserver
	virtual ~InputStrategy() = default;
	virtual bool OnKeyPress(uint32_t asci) { return false; }
	virtual bool OnMouseMove(uint32_t x, uint32_t y) { return false; }
	virtual bool OnMousePress(uint32_t x, uint32_t y, bool left) { return false; }
	virtual bool OnMouseRelease() { return false; }

	virtual void UpdateInRenderThread() {}
};

//-------------------------------------------------------------------------------
class InputStrategyMoveAlongXZPlane : public InputStrategy
{
public:
	InputStrategyMoveAlongXZPlane(Camera& _camera, std::vector<shObject> _objs)
		:m_camera(_camera)
		,m_objects(_objs) // @todo need to subscribe on update objects !!!
	{

	}

	virtual bool OnMousePress(uint32_t _x, uint32_t _y, bool _left)
	{
		auto [new_focused, intersaction] = m_camera.get().getCameraRay().calculateIntersaction(m_objects);
		if (!_left && new_focused) // grab if left is pressed
		{
			m_grab_translation = new_focused->GetTransform()->getTranslation();
			m_intersaction = intersaction;
			m_grab_camera_line = m_camera.get().getCameraRay().getLine();
			m_dragged = new_focused;
			return true;
		}
		return false;
	}

	virtual bool OnMouseMove(uint32_t _x, uint32_t _y)
	{
		if (m_grab_camera_line != std::nullopt) // button is pressed
		{
			m_camera.get().getCameraRay().Update(m_camera.get(),
																					 static_cast<float>(_x),
																					 static_cast<float>(_y),
																					 static_cast<float>(m_camera.get().getWidth()),
																					 static_cast<float>(m_camera.get().getHeight()));
			m_camera.get().getCameraRay().press((float)_x, (float)_y);
			
			dbb::plane pl(m_intersaction,
				glm::vec3(0.0f, m_intersaction.y, 1.0f),
				glm::vec3(1.0f, m_intersaction.y, 0.0f)); // arbitrary triangle on xz plane
			glm::vec3 new_intersaction = dbb::intersection(pl, m_camera.get().getCameraRay().getLine());
			m_translation_vector = new_intersaction - m_intersaction;
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual bool OnMouseRelease()
	{
		m_grab_camera_line = std::nullopt;
		m_translation_vector = vec3{ 0.0f, 0.0f, 0.0f };
		return true;
	}

	virtual void UpdateInRenderThread()
	{
		if (m_grab_camera_line != std::nullopt && m_translation_vector != glm::vec3{ 0.f,0.f,0.0f })
			m_dragged->GetTransform()->setTranslation(m_grab_translation + m_translation_vector);
	}

protected:
	std::reference_wrapper<Camera>	m_camera;
	std::vector<shObject>						m_objects;
	shObject												m_dragged;
	std::optional<dbb::line>				m_grab_camera_line = std::nullopt;
	glm::vec3												m_intersaction;
	glm::vec3												m_grab_translation;
	glm::vec3												m_translation_vector = glm::vec3{ 0.f,0.f,0.0f };
};

//---------------------------------------------------------------
class InputStrategy2DMove : public InputStrategy
{
public:
	InputStrategy2DMove(IGame*_game) :m_game(_game) {}

	virtual bool OnMouseMove(uint32_t _x, uint32_t _y) 
	{
		if (m_grabbed != nullptr)
		{
			float x_ss = (((float)_x / (float)m_game->Width()) - 0.5f) * 2.0f;
			float y_ss = -(((float)_y / (float)m_game->Height()) - 0.5f) * 2.0f;
			m_grabbed->GetTransform()->setTranslation({ x_ss, y_ss , 0.0f });
			return true;
		}
		return false;
	}

	virtual bool OnMousePress(uint32_t _x, uint32_t _y, bool _left)
	{ 
		if (!_left)
		{
			std::vector<std::shared_ptr<eObject>> objs = GetObjectsWithChildren(m_game->GetObjects());
			float x_ss = (((float)_x / (float)m_game->Width()) - 0.5f) * 2.0f;
			float y_ss = -(((float)_y / (float)m_game->Height()) - 0.5f) * 2.0f;
			for (auto& obj : objs)
			{
				if (obj->Is2DScreenSpace())
				{
					float delta_x = abs(obj->GetTransform()->getTranslation().x - x_ss);
					float delta_y = abs(obj->GetTransform()->getTranslation().y - y_ss);
					if (delta_x < 0.01f && delta_y < 0.01f)
					{
						m_grabbed = obj;
						return true;
					}
				}
			}
		}
		return false;
	}

	virtual bool OnMouseRelease()
	{
		m_grabbed = nullptr;
		return false;
	}

protected:
	IGame* m_game = nullptr;
	shObject m_grabbed = nullptr;
};

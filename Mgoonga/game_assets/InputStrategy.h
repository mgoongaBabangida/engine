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
																					 m_camera.get().getWidth(),
																					 m_camera.get().getHeight());
			m_camera.get().getCameraRay().press(_x, _y);
			
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

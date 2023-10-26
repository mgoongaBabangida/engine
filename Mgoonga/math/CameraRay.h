#pragma once

#include <base/Object.h>

#include "math.h"

#include "Transform.h"
#include "PlaneLine.h"

class Camera;

namespace dbb
{
	//------------------------------------------------------------------------------------
	class DLL_MATH CameraRay
	{
	public:
		explicit CameraRay(Camera&);
		CameraRay() = default;

		void					                   Update(float click_x, float click_y);
		std::pair<shObject, glm::vec3>   calculateIntersaction(std::vector<shObject> objects);
    bool									           IsPressed() const { return pressed; }
		std::pair<glm::vec2, glm::vec2>  GetFrame() const { return std::pair<glm::vec2, glm::vec2>{ press_start, press_curr }; }
		std::vector<shObject>            onMove(std::vector<shObject> objects, float click_x, float click_y);
		std::vector<shObject>						 FrustumCull(std::vector<shObject> _objects);

		dbb::line				      getLine() const { return m_line; }
		void					        press(float click_x, float click_y);
		void					        release();

	protected:
		dbb::line				_getLine(glm::vec2);
		bool					  isOpSign(float, float);
		
		std::reference_wrapper<Camera> m_camera;

		dbb::line			m_line;
		Transform			m_transform;

		bool					pressed = false;
		glm::vec2			press_start;
		glm::vec2			press_curr;
	};
}


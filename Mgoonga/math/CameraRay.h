#pragma once

#include <base/interfaces.h>
#include <base/Object.h>

#include "math.h"

#include "Transform.h"
#include "PlaneLine.h"
#include "Camera.h"

namespace dbb
{
	//------------------------------------------------------------------------------------
	class DLL_MATH CameraRay : public IInputObserver
	{
	public:
		CameraRay(float w,float h, float n,float f, float pers_angle);
		CameraRay() = default;

		void					                   init(float width, float height, float near, float far, float pers = 60.0f);
		void					                   Update(Camera& camera, float click_x, float click_y, float w, float h);
		shObject				                 calculateIntersaction(std::vector<shObject> objects);
    bool									           IsPressed() const { return pressed; }
		std::pair<glm::vec2, glm::vec2>  GetFrame() const { return std::pair<glm::vec2, glm::vec2>{ press_start, press_curr }; }
		std::vector<shObject>            onMove(Camera& camera, std::vector<shObject> objects, float click_x, float click_y);
		
		dbb::line				      getLine()const { return m_line; }
		void					        press(float click_x, float click_y);
		void					        release();

	protected:
		dbb::line				getLine(Camera& camera, glm::vec2);
		bool					  isOpSign(float, float);
		
		float					perspective_angle_degrees;
		float					width;
		float					height;
		float					near_plane;
		float					far_plane;
		dbb::line			m_line;
		Transform			transform;
		bool					pressed = false;
		glm::vec2			press_start;
		glm::vec2			press_curr;
	};
}


#pragma once
#include "Camera.h"
#include "Structures.h"
#include "Transform.h"

namespace dbb
{
	class CameraRay
	{
	public:
		CameraRay(float w,float h, float n,float f, float pers_angle);
		CameraRay() {}

		void					init(float Width, float Height, float near, float far, float pers = 60.0f);
		void					Update(Camera& camera, float click_x, float click_y, float w, float h);
		shObject				calculateIntersaction(std::vector<shObject> objects);
		std::vector<shObject>	onMove(Camera& camera, std::vector<shObject> objects, float click_x, float click_y);
		dbb::line				getLine()const { return m_line; }
		void					press(float click_x, float click_y);
		void					release();

	protected:
		dbb::line getLine(Camera& camera, glm::vec2);
		bool isOpSign(float, float);
		
		float		perspective_angle_degrees;
		float		Width;
		float		Height;
		float		near_plane;
		float		far_plane;
		dbb::line	m_line;
		Transform	transform;
		bool		pressed;
		glm::vec2	press_start;
		glm::vec2	press_curr;
	};
}

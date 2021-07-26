#include "stdafx.h"
#include "CameraRay.h"
#include "BoxCollider.h"

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include <glm\glm/gtc/constants.hpp>
#include <map>

namespace dbb
{

	//-------------------------------------------------------------------------------
	CameraRay::CameraRay(float w, float h, float n, float f, float pers_angle)
		: width(w),
		  height(h),
		  near_plane(n),
		  far_plane(f),
		  perspective_angle_degrees(pers_angle)
	{}

	void CameraRay::init(float _width, float _height, float _near, float _far, float _pers_angle)
	{
		width = _width;
		height = _height;
		near_plane = _near;
		far_plane = _far;
		perspective_angle_degrees = _pers_angle;
	}

	void CameraRay::Update(Camera& camera, float click_x, float click_y, float w, float h)
	{
		//glm::quat rot = Transform::RotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), camera.getDirection()); //?
		//this->transform.setRotation(rot);
		this->transform.billboard(camera.getDirection());
		this->transform.setTranslation(camera.getPosition());
		this->width = w;
		this->height = h;

		float XOffsetCoef = click_x / width;
		float YOffsetCoef = click_y / height;
		float heightN = 2 * tan(glm::radians(perspective_angle_degrees / 2)) * near_plane;  //? glm::radians?
		float heightF = 2 * tan(glm::radians(perspective_angle_degrees / 2)) * far_plane;   //?
		float widthN = heightN* (width / height);
		float widthF = heightF* (width / height);

		glm::vec3 dot1 = glm::vec3(widthN / 2 - XOffsetCoef * widthN, heightN / 2 - YOffsetCoef * heightN, near_plane);
		glm::vec3 dot2 = glm::vec3(widthF / 2 - XOffsetCoef * widthF, heightF / 2 - YOffsetCoef * heightF, far_plane);  // plus or minus Z ?

		glm::vec3 dir = dot2 - dot1;
		m_line.M = transform.getModelMatrix() * glm::vec4(dot1, 1.0f); //origin
		m_line.p = glm::mat3(transform.getRotation()) * dir;  //direction

		dot1 = transform.getModelMatrix()* glm::vec4(dot1, 1.0f);
		dot2 = transform.getModelMatrix()* glm::vec4(dot2, 1.0f);

#ifdef DEBUG_CAMERA_RAY
		std::cout << "dot1=" << dot1.x << " " << dot1.y << " " << dot1.z << std::endl;
		std::cout << "dot2= " << dot2.x << " " << dot2.y << " " << dot2.z << std::endl;
		std::cout << "cam=" << camera.getDirection().x << " " << camera.getDirection().y << " " << camera.getDirection().z << std::endl;
		std::cout << "trans=" << transform.getRotationVector().x << " " << transform.getRotationVector().y << " " << transform.getRotationVector().z << std::endl;
		std::cout << "camPos=" << camera.getPosition().x << " " << camera.getPosition().y << " " << camera.getPosition().z << std::endl;
#endif // DEBUG
	}

	shObject CameraRay::calculateIntersaction(std::vector<shObject> objects)
	{
		std::multimap<shObject, glm::vec3> intersections;
		int debug_counter = 0;
		for (auto &obj : objects)
		{
			std::vector<glm::mat3> boundings = obj->GetCollider()->GetBoundingTriangles(*(obj->GetTransform()));
			line line;
			line.p = glm::vec4(m_line.p, 1.0f);
			line.M = glm::vec4(m_line.M, 1.0f);

			for (auto& triangle : boundings)
			{
				plane plane(triangle);
				glm::vec3 inters = intersection(plane, line);
				if (IsInside(triangle, inters) /*&& glm::dot(line.p, glm::vec3(inters - line.M))> 0.0f*/) //check if behind 
				{
					intersections.insert(std::pair<shObject, glm::vec3>(obj, inters));
				}
			}
			++debug_counter;
		}
		if (intersections.empty())
		{
			return nullptr;
		}
		else
		{
			float length = 1000.0f;  //should be in line with far plane!!!!!!!
			shObject obj = nullptr;
			for (auto &inter : intersections)
			{
				if (glm::length(m_line.M - inter.second) < length)
				{
					length = glm::length(m_line.M - inter.second);
					obj = inter.first;
				}
			}
			return obj;
		}
	}

	std::vector<shObject> CameraRay::onMove(Camera& camera, std::vector<shObject> objects, float click_x, float click_y)
	{
		press_curr = glm::vec2(click_x, click_y);
		std::vector<shObject> ret;
		if (pressed)
		{
			line  line1 = getLine(camera, press_start);
			line  line2 = getLine(camera, press_curr);
			line  line3 = getLine(camera, glm::vec2(press_start.x, press_curr.y));
			line  line4 = getLine(camera, glm::vec2(press_curr.x, press_start.y));
			plane left(line1.M, line3.M, line1.M + line1.p);
			plane right(line2.M, line4.M, line2.M + line2.p);
			plane top(line1.M, line4.M, line1.M + line1.p);
			plane bottom(line2.M, line3.M, line2.M + line2.p);

			for (auto &obj : objects)
			{
				std::vector<glm::vec3> extrems = obj->GetCollider()->GetExtrems(*(obj->GetTransform()));
				for (auto &extrem : extrems)
				{
					if (!isOpSign(left.A * extrem.x + left.B * extrem.y + left.C * extrem.z + left.D,
						right.A * extrem.x + right.B * extrem.y + right.C * extrem.z + right.D) &&
						!isOpSign(top.A * extrem.x + top.B * extrem.y + top.C * extrem.z + top.D,
							bottom.A * extrem.x + bottom.B * extrem.y + bottom.C * extrem.z + bottom.D))
					{
						ret.push_back(obj);
						break;
					}
				}
			}
			//std::cout << "Grabed " << ret.size() << " Objects" << std::endl;
		}
		return ret;
	}

	void CameraRay::press(float click_x, float click_y)
	{
		press_start = press_curr = glm::vec2(click_x, click_y);
		pressed = true;
	}

	void CameraRay::release()
	{
		press_start = press_curr = glm::vec2(-1, -1);
		pressed = false;
	}

	line CameraRay::getLine(Camera& camera, glm::vec2 click)
	{
		line line;
		this->transform.billboard(camera.getDirection());
		this->transform.setTranslation(camera.getPosition());

		float XOffsetCoef = click.x / width;
		float YOffsetCoef = click.y / height;
		float heightN = 2 * tan(glm::radians(perspective_angle_degrees / 2)) * near_plane;  //? glm::radians?
		float heightF = 2 * tan(glm::radians(perspective_angle_degrees / 2)) * far_plane;   //?
		float widthN = heightN* (width / height);
		float widthF = heightF* (width / height);

		glm::vec3 dot1 = glm::vec3(widthN / 2 - XOffsetCoef * widthN, heightN / 2 - YOffsetCoef * heightN, near_plane);
		glm::vec3 dot2 = glm::vec3(widthF / 2 - XOffsetCoef * widthF, heightF / 2 - YOffsetCoef * heightF, far_plane);  // plus or minus Z ?

		glm::vec3 dir = dot2 - dot1;
		line.M = transform.getModelMatrix() * glm::vec4(dot1, 1.0f); //origin
		line.p = glm::mat3(transform.getRotation()) * dir;  //direction

		return line;
	}

	bool CameraRay::isOpSign(float a, float b)
	{
		if (a > 0 && b < 0)
			return true;
		if (b > 0 && a < 0)
			return true;
		return false;
	}

}


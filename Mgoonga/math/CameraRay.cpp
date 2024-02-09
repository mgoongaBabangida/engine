#include "stdafx.h"
#include "CameraRay.h"
#include "BoxCollider.h"
#include "Camera.h"

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include <glm\glm/gtc/constants.hpp>
#include <map>

namespace dbb
{

	//-------------------------------------------------------------------------------
	CameraRay::CameraRay(Camera& _camera)
		: m_camera(_camera)
	{}

	//------------------------------------------------------------------------------------------------
	void CameraRay::Update(float click_x, float click_y)
	{
		//glm::quat rot = Transform::RotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), camera.getDirection()); //?
		//this->transform.setRotation(rot);
		const Camera& camera = m_camera.get();
		this->m_transform.billboard(camera.getDirection());
		this->m_transform.setTranslation(camera.getPosition());
		float w = camera.getWidth();
		float h = camera.getHeight();

		float XOffsetCoef = click_x / w;
		float YOffsetCoef = click_y / h;

		float heightN = 2 * tan(glm::radians(camera.getZoom() / 2)) * camera.getNearPlane();  //? glm::radians?
		float heightF = 2 * tan(glm::radians(camera.getZoom() / 2)) * camera.getFarPlane();   //?
		float widthN = heightN * (camera.getWidth() / camera.getHeight());
		float widthF = heightF * (camera.getWidth() / camera.getHeight());

		glm::vec3 dot1 = glm::vec3(widthN / 2 - XOffsetCoef * widthN, heightN / 2 - YOffsetCoef * heightN, camera.getNearPlane());
		glm::vec3 dot2 = glm::vec3(widthF / 2 - XOffsetCoef * widthF, heightF / 2 - YOffsetCoef * heightF, camera.getFarPlane());  // plus or minus Z ?

		glm::vec3 dir = dot2 - dot1;
		m_line.M = m_transform.getModelMatrix() * glm::vec4(dot1, 1.0f); //origin
		m_line.p = glm::mat3(m_transform.getRotation()) * dir;  //direction

		dot1 = m_transform.getModelMatrix()* glm::vec4(dot1, 1.0f);
		dot2 = m_transform.getModelMatrix()* glm::vec4(dot2, 1.0f);
	}

	//------------------------------------------------------------------------------------------------
	std::pair<shObject, glm::vec3> CameraRay::calculateIntersaction(std::vector<shObject> objects)
	{
		std::multimap<shObject, glm::vec3> intersections;

		for (auto &obj : objects)
		{
			if (!obj->GetCollider() || !obj->IsPickable())
				continue;

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
		}

		if (intersections.empty())
		{
			return {};
		}
		else
		{
			float length = 1000.0f;  //@ todo should be in line with far plane
			std::pair<shObject, glm::vec3> obj;
			for (auto &inter : intersections)
			{
				if (glm::length(m_line.M - inter.second) < length)
				{
					length = glm::length(m_line.M - inter.second);
					obj = inter;
				}
			}
			return obj;
		}
	}

	//------------------------------------------------------------------------------------------------
	std::vector<shObject> CameraRay::onMove(std::vector<shObject> objects, float click_x, float click_y)
	{
		const Camera& camera = m_camera.get();
		press_curr = glm::vec2(click_x, click_y);
		std::vector<shObject> ret;
		if (pressed)
		{
			line  line1 = _getLine(press_start);
			line  line2 = _getLine(press_curr);
			line  line3 = _getLine(glm::vec2(press_start.x, press_curr.y));
			line  line4 = _getLine(glm::vec2(press_curr.x, press_start.y));

			plane left(line1.M, line3.M, line1.M + line1.p);
			plane right(line2.M, line4.M, line2.M + line2.p);
			plane top(line1.M, line4.M, line1.M + line1.p);
			plane bottom(line2.M, line3.M, line2.M + line2.p);

			for (auto &obj : objects)
			{
				if (!obj->GetCollider())
					continue;

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

	//------------------------------------------------------------------------------------------------
	std::vector<shObject> CameraRay::FrustumCull(std::vector<shObject> _objects)
	{
		const Camera& camera = m_camera.get();
		std::vector<shObject> ret;

		line  line1 = _getLine({0, 0});
		line  line2 = _getLine({camera.getWidth(), camera.getHeight()});
		line  line3 = _getLine(glm::vec2(0, camera.getHeight()));
		line  line4 = _getLine(glm::vec2(camera.getWidth(), 0));

		plane left(line1.M, line3.M, line1.M + line1.p);
		plane right(line2.M, line4.M, line2.M + line2.p);
		plane top(line1.M, line4.M, line1.M + line1.p);
		plane bottom(line2.M, line3.M, line2.M + line2.p);
		plane back(line1.M, line2.M, line3.M);
		plane front(line1.M + line1.p, line2.M + line2.p, line3.M + line3.p);

		for (auto& obj : _objects)
		{
			if (!obj->GetCollider())
				continue;

			if(obj->Name() == "GrassPlane") //@todo temp degub
				continue;

			std::vector<glm::vec3> extrems = obj->GetCollider()->GetExtrems(*(obj->GetTransform()));
			for (auto& extrem : extrems)
			{
				if (!isOpSign(left.A * extrem.x + left.B * extrem.y + left.C * extrem.z + left.D,
											right.A * extrem.x + right.B * extrem.y + right.C * extrem.z + right.D) &&
					  !isOpSign(top.A * extrem.x + top.B * extrem.y + top.C * extrem.z + top.D,
											bottom.A * extrem.x + bottom.B * extrem.y + bottom.C * extrem.z + bottom.D) &&
						 isOpSign(back.A * extrem.x + back.B * extrem.y + back.C * extrem.z + back.D,
											front.A * extrem.x + front.B * extrem.y + front.C * extrem.z + front.D))
				{
					ret.push_back(obj);
					break;
				}
			}
		}
		return ret;
	}

	//------------------------------------------------------------------------------------------------
	bool	CameraRay::IsInFrustum(const std::vector<glm::vec3>& _extrems)
	{
		if (_extrems.size() != 8)
			return false;  // error

		const Camera& camera = m_camera.get();

		line  line1 = _getLine({ 0, 0 });
		line  line2 = _getLine({ camera.getWidth(), camera.getHeight() });
		line  line3 = _getLine(glm::vec2(0, camera.getHeight()));
		line  line4 = _getLine(glm::vec2(camera.getWidth(), 0));

		plane left(line1.M, line3.M, line1.M + line1.p);
		plane right(line2.M, line4.M, line2.M + line2.p);
		plane top(line1.M, line4.M, line1.M + line1.p);
		plane bottom(line2.M, line3.M, line2.M + line2.p);
		plane back(line1.M, line2.M, line3.M);
		plane front(line1.M + line1.p, line2.M + line2.p, line3.M + line3.p);

		for (auto& extrem : _extrems)
		{
			if (!isOpSign(left.A * extrem.x + left.B * extrem.y + left.C * extrem.z + left.D,
				right.A * extrem.x + right.B * extrem.y + right.C * extrem.z + right.D) &&
				!isOpSign(top.A * extrem.x + top.B * extrem.y + top.C * extrem.z + top.D,
					bottom.A * extrem.x + bottom.B * extrem.y + bottom.C * extrem.z + bottom.D) &&
				isOpSign(back.A * extrem.x + back.B * extrem.y + back.C * extrem.z + back.D,
					front.A * extrem.x + front.B * extrem.y + front.C * extrem.z + front.D))
				return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------
	void CameraRay::press(float click_x, float click_y)
	{
		press_start = press_curr = glm::vec2(click_x, click_y);
		pressed = true;
	}

	//------------------------------------------------------------------------------------------------
	void CameraRay::release()
	{
		press_start = press_curr = glm::vec2(-1, -1);
		pressed = false;
	}

	//------------------------------------------------------------------------------------------------
	line CameraRay::_getLine(glm::vec2 click)
	{
		const Camera& camera = m_camera.get();
		line line;
		m_transform.billboard(camera.getDirection());
		m_transform.setTranslation(camera.getPosition());

		float XOffsetCoef = click.x / camera.getWidth();
		float YOffsetCoef = click.y / camera.getHeight();
		float heightN = 2 * tan(glm::radians(camera.getZoom() / 2)) * camera.getNearPlane();  //? glm::radians?
		float heightF = 2 * tan(glm::radians(camera.getZoom() / 2)) * camera.getFarPlane();   //?
		float widthN = heightN* (camera.getWidth() / camera.getHeight());
		float widthF = heightF* (camera.getWidth() / camera.getHeight());

		glm::vec3 dot1 = glm::vec3(widthN / 2 - XOffsetCoef * widthN, heightN / 2 - YOffsetCoef * heightN, camera.getNearPlane());
		glm::vec3 dot2 = glm::vec3(widthF / 2 - XOffsetCoef * widthF, heightF / 2 - YOffsetCoef * heightF, camera.getFarPlane());  // plus or minus Z ?

		glm::vec3 dir = dot2 - dot1;
		line.M = m_transform.getModelMatrix() * glm::vec4(dot1, 1.0f); //origin
		line.p = glm::mat3(m_transform.getRotation()) * dir;  //direction (should not be normalized to use it to find dot on far plane)

		return line;
	}

	//------------------------------------------------------------------------------------------------
	bool CameraRay::isOpSign(float a, float b)
	{
		if (a > 0 && b < 0)
			return true;
		if (b > 0 && a < 0)
			return true;
		return false;
	}

}


#include "stdafx.h"
#include "CameraRay.h"

dbb::CameraRay::CameraRay(float w, float h, float n, float f,float pers_angle )
: Width(w)
, Height(h)
, near_plane(n)
, far_plane(f)
, perspective_angle_degrees(pers_angle)
{}

void dbb::CameraRay::init(float _width, float _height, float _near, float _far, float _pers_angle)
{
	Width = _width;
	Height = _height;
	near_plane = _near;
	far_plane = _far;
	perspective_angle_degrees = _pers_angle;
}

void dbb::CameraRay::Update(Camera& camera, float click_x, float click_y,float w,float h)
{
	//glm::quat rot = Transform::RotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), camera.getDirection()); //?
	//this->transform.setRotation(rot);
	this->transform.billboard(camera.getDirection());
	this->transform.setTranslation(camera.getPosition());
	this->Width = w;
	this->Height = h;

	float XOffsetCoef = click_x / Width;
	float YOffsetCoef = click_y / Height;
	float heightN = 2 * tan(glm::radians(perspective_angle_degrees/2)) * near_plane;  //? glm::radians?
	float heightF = 2 * tan(glm::radians(perspective_angle_degrees/2)) * far_plane;   //?
	float widthN = heightN* (Width /Height);
	float widthF = heightF* (Width /Height);

	glm::vec3 dot1 = glm::vec3( widthN / 2 - XOffsetCoef * widthN, heightN / 2 - YOffsetCoef * heightN, near_plane);
	glm::vec3 dot2 = glm::vec3( widthF / 2 - XOffsetCoef * widthF, heightF / 2 - YOffsetCoef * heightF, far_plane);  // plus or minus Z ?

	glm::vec3 dir = dot2 - dot1;
	m_line.M = transform.getModelMatrix() * glm::vec4(dot1, 1.0f); //origin
	m_line.p = glm::mat3(transform.getRotation()) * dir ;  //direction

	dot1 = transform.getModelMatrix()* glm::vec4(dot1, 1.0f);
	dot2 = transform.getModelMatrix()* glm::vec4(dot2, 1.0f);

#ifdef DEBUG_CAMERA_RAY
	std::cout <<"dot1="<< dot1.x <<" "<< dot1.y << " "<<dot1.z << std::endl;
	std::cout << "dot2= "<< dot2.x << " "<< dot2.y <<" "<< dot2.z << std::endl;
	std::cout << "cam=" << camera.getDirection().x << " " << camera.getDirection().y << " " << camera.getDirection().z << std::endl;
	std::cout << "trans=" << transform.getRotationVector().x << " " << transform.getRotationVector().y << " " << transform.getRotationVector().z << std::endl;
	std::cout << "camPos=" << camera.getPosition().x << " " << camera.getPosition().y << " " << camera.getPosition().z << std::endl;
#endif // DEBUG
}

shObject dbb::CameraRay::calculateIntersaction(std::vector<shObject> objects)
{
#ifdef DEBUG_CAMERA_RAY
	std::cout << "----Try-----------" << std::endl;
#endif // DEBUG
	std::multimap<shObject, glm::vec3> intersections;
	int debug_counter = 0;
	for (auto &obj : objects) 
	{
		std::vector<glm::mat3> boundings = obj->getCollider()->getBoundingTriangles(*(obj->getTransform()));
		dbb::line line;
		line.p = glm::vec4(m_line.p, 1.0f);
		line.M = glm::vec4(m_line.M, 1.0f);

		for (auto& triangle : boundings) 
		{
			dbb::plane plane(triangle);
			glm::vec3 inters = dbb::intersection(plane, line);
			if (dbb::IsInside(triangle, inters) /*&& glm::dot(line.p, glm::vec3(inters - line.M))> 0.0f*/ ) //check if behind 
			{
				intersections.insert(std::pair<shObject, glm::vec3>(obj, inters));
#ifdef DEBUG_CAMERA_RAY
				std::cout << "Object= " << debug_counter << std::endl;
#endif
			}
		}
		++debug_counter;
	}
	if (intersections.empty())
	{
#ifdef DEBUG_CAMERA_RAY
		std::cout << "No intersactions found!" << std::endl;
#endif
		return nullptr;
	}
	else 
	{
#ifdef DEBUG_CAMERA_RAY
		std::cout << "Intersactions found!!!" << std::endl;
#endif
		float length = 1000.0f;  //should be in line with far plane!!!!!!!
		shObject obj = nullptr;
		for (auto &inter : intersections)
		{
			if (glm::length(m_line.M - inter.second) < length) 
			{ 
				length = glm::length( m_line.M - inter.second);
				obj = inter.first;
#ifdef DEBUG_CAMERA_RAY
				std::cout << "length=" << length << std::endl;
#endif
			}
		}
		return obj;
	}
}

std::vector<shObject> dbb::CameraRay::onMove(Camera& camera, std::vector<shObject> objects, float click_x, float click_y)
{
	press_curr = glm::vec2(click_x, click_y);
	std::vector<shObject> ret;
	if (pressed)
	{
		dbb::line  line1 = getLine(camera, press_start);
		dbb::line  line2 = getLine(camera, press_curr);
		dbb::line  line3 = getLine(camera, glm::vec2(press_start.x, press_curr.y));
		dbb::line  line4 = getLine(camera, glm::vec2(press_curr.x, press_start.y));
		dbb::plane left(line1.M, line3.M, line1.M + line1.p);
		dbb::plane right(line2.M, line4.M, line2.M + line2.p);
		dbb::plane top(line1.M, line4.M, line1.M + line1.p);
		dbb::plane bottom(line2.M, line3.M, line2.M + line2.p);

		for (auto &obj : objects)
		{
			std::vector<glm::vec3> extrems = obj->getCollider()->getExtrems(*(obj->getTransform()));
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
		std::cout << "Grabed " << ret.size() << " Objects" << std::endl;
	}
	return ret;
}

void dbb::CameraRay::press(float click_x, float click_y)
{
	press_start = glm::vec2(click_x, click_y);
	pressed = true;
}

void dbb::CameraRay::release()
{
	press_start = glm::vec2(-1, -1);
	pressed = false;
}

dbb::line dbb::CameraRay::getLine(Camera& camera, glm::vec2 click)
{
	dbb::line line;
	this->transform.billboard(camera.getDirection());
	this->transform.setTranslation(camera.getPosition());

	float XOffsetCoef = click.x / Width;
	float YOffsetCoef = click.y / Height;
	float heightN = 2 * tan(glm::radians(perspective_angle_degrees / 2)) * near_plane;  //? glm::radians?
	float heightF = 2 * tan(glm::radians(perspective_angle_degrees / 2)) * far_plane;   //?
	float widthN = heightN* (Width / Height);
	float widthF = heightF* (Width / Height);

	glm::vec3 dot1 = glm::vec3(widthN / 2 - XOffsetCoef * widthN, heightN / 2 - YOffsetCoef * heightN, near_plane);
	glm::vec3 dot2 = glm::vec3(widthF / 2 - XOffsetCoef * widthF, heightF / 2 - YOffsetCoef * heightF, far_plane);  // plus or minus Z ?

	glm::vec3 dir = dot2 - dot1;
	line.M = transform.getModelMatrix() * glm::vec4(dot1, 1.0f); //origin
	line.p = glm::mat3(transform.getRotation()) * dir;  //direction

	return line;
}

bool dbb::CameraRay::isOpSign(float a, float b)
{
	if (a > 0 && b < 0)
		return true;
	if (b > 0 && a < 0)
		return true;
	return false;
}

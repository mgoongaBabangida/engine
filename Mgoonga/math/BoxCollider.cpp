#include "stdafx.h"
#include "BoxCollider.h"

#include <glm\glm\gtx\norm.hpp>

//-------------------------------------------------------------------------------
void BoxCollider::CalculateExtremDots(const std::vector<glm::vec3>& positions)
{
	dots.MaxX = -1000, dots.MinX = 1000, dots.MaxY = -1000, dots.MinY = 1000, dots.MaxZ = -1000, dots.MinZ = 1000;  //max min float ?
	for (int i = 0; i < positions.size(); ++i)
	{
		if (positions[i].x > dots.MaxX)
			dots.MaxX = positions[i].x;
		if (positions[i].x < dots.MinX)
			dots.MinX = positions[i].x;
		if (positions[i].y > dots.MaxY)
			dots.MaxY = positions[i].y;
		if (positions[i].y < dots.MinY)
			dots.MinY = positions[i].y;
		if (positions[i].z > dots.MaxZ)
			dots.MaxZ = positions[i].z;
		if (positions[i].z < dots.MinZ)
			dots.MinZ = positions[i].z;
	}
	m_center = GetCenter();
}

//-------------------------------------------------------------------------------
std::vector<glm::mat3> BoxCollider::GetBoundingTriangles(const ITransform& trans) const
{
	//extrem dots should exist
	glm::mat4 transform = trans.getModelMatrix();
	std::vector<glm::mat3> ret; // Getting 12 triangles of the bouning cube
	ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f)),
													glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f)),
													glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MinX, dots.MinY, dots.MaxZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MaxX, dots.MinY, dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MinX, dots.MinY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MinX, dots.MinY, dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MinX, dots.MinY, dots.MaxZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MaxX, dots.MinY, dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MaxX, dots.MinY, dots.MaxZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MaxX, dots.MaxY, dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MinX, dots.MinY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MaxX, dots.MinY, dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MaxX, dots.MaxY, dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MinX, dots.MinY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MinX, dots.MinY, dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MinX, dots.MinY, dots.MinZ, 1.0f)),
							glm::vec3(transform *glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f))));

	return ret;
}

//-------------------------------------------------------------------------------
std::vector<glm::vec3> BoxCollider::GetExtrems(const ITransform& trans) const
{
	glm::mat4 transform = trans.getModelMatrix();
	std::vector<glm::vec3> ret;
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MinX, dots.MinY, dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MinX, dots.MinY, dots.MaxZ, 1.0f)));
	return ret;
}

//-------------------------------------------------------------------------------
std::vector<glm::mat3> BoxCollider::GetBoundingTrianglesLocalSpace() const
{
	//extrem dots should exist
	std::vector<glm::mat3> ret; // Getting 12 triangles of the bouning cube
	ret.push_back(glm::mat3(glm::vec3(dots.MaxX, dots.MaxY, dots.MinZ),
													glm::vec3(dots.MinX, dots.MaxY, dots.MaxZ),
													glm::vec3(dots.MaxX, dots.MaxY, dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(dots.MaxX, dots.MaxY, dots.MinZ),
													glm::vec3(dots.MinX, dots.MaxY, dots.MinZ),
													glm::vec3(dots.MinX, dots.MaxY, dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(dots.MaxX, dots.MinY, dots.MinZ),
													glm::vec3(dots.MinX, dots.MinY, dots.MaxZ),
													glm::vec3(dots.MaxX, dots.MinY, dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(dots.MaxX, dots.MinY, dots.MinZ),
													glm::vec3(dots.MinX, dots.MinY, dots.MinZ),
													glm::vec3(dots.MinX, dots.MinY, dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(dots.MinX, dots.MaxY, dots.MaxZ),
													glm::vec3(dots.MinX, dots.MinY, dots.MaxZ),
													glm::vec3(dots.MaxX, dots.MinY, dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(dots.MinX, dots.MaxY, dots.MaxZ),
													glm::vec3(dots.MaxX, dots.MinY, dots.MaxZ),
													glm::vec3(dots.MaxX, dots.MaxY, dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(dots.MinX, dots.MaxY, dots.MinZ),
													glm::vec3(dots.MinX, dots.MinY, dots.MinZ),
													glm::vec3(dots.MaxX, dots.MinY, dots.MinZ)));

	ret.push_back(glm::mat3(glm::vec3(dots.MinX, dots.MaxY, dots.MinZ),
													glm::vec3(dots.MaxX, dots.MinY, dots.MinZ),
													glm::vec3(dots.MaxX, dots.MaxY, dots.MinZ)));

	ret.push_back(glm::mat3(glm::vec3(dots.MaxX, dots.MaxY,dots.MinZ),
													glm::vec3(dots.MaxX, dots.MinY, dots.MinZ),
													glm::vec3(dots.MaxX, dots.MinY, dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(dots.MaxX, dots.MaxY, dots.MinZ),
													glm::vec3(dots.MaxX, dots.MinY, dots.MinZ),
													glm::vec3(dots.MaxX, dots.MaxY, dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(dots.MinX, dots.MaxY, dots.MinZ),
													glm::vec3(dots.MinX, dots.MinY, dots.MinZ),
													glm::vec3(dots.MinX, dots.MinY, dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(dots.MinX, dots.MaxY, dots.MinZ),
													glm::vec3(dots.MinX, dots.MinY, dots.MinZ),
													glm::vec3(dots.MinX, dots.MaxY, dots.MaxZ)));
	return ret;
}

//-------------------------------------------------------------------------------
std::vector<glm::vec3> BoxCollider::GetExtremsLocalSpace() const
{
	std::vector<glm::vec3> ret;
	ret.push_back(glm::vec3(glm::vec4(dots.MaxX, dots.MaxY, dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(dots.MaxX, dots.MinY, dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(dots.MinX, dots.MinY, dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(dots.MinX, dots.MinY, dots.MaxZ, 1.0f)));
	return ret;
}

//-------------------------------------------------------------------------------
glm::vec3 BoxCollider::GetCenter()
{
	if (m_center == std::nullopt)
	{
		m_center = glm::vec3(dots.MaxX - glm::length(dots.MaxX - dots.MinX) / 2,
												 dots.MaxY - glm::length(dots.MaxY - dots.MinY) / 2,
												 dots.MaxZ - glm::length(dots.MaxZ - dots.MinZ) / 2);// always == line.M
	}
	return *m_center;
}

//-------------------------------------------------------------------------------
std::vector<dbb::line> BoxCollider::getRays(const ITransform & trans, Side moveDirection, std::vector<float>& lengths)
{
	std::vector<dbb::line> rays;
	glm::vec3 fwd, fwd_dwn_l, fwd_dwn_r, fwd_up_l, fwd_up_r;
	switch (moveDirection) 
	{
	case FORWARD:
		fwd			= trans.getForward(); //suppose +Z  1 
		fwd_dwn_l	= glm::vec3(dots.MaxX, dots.MinY, dots.MaxZ);  //2
		fwd_dwn_r	= glm::vec3(dots.MinX, dots.MinY, dots.MaxZ);  //3
		fwd_up_l	= glm::vec3(dots.MaxX, dots.MaxY, dots.MaxZ);  //4
		fwd_up_r	= glm::vec3(dots.MinX, dots.MaxY, dots.MaxZ);  //5
		break;

	case BACK:
		fwd			= -trans.getForward(); //suppose +Z  1 
		fwd_dwn_l	= glm::vec3(dots.MaxX, dots.MinY, dots.MinZ);  //2
		fwd_dwn_r	= glm::vec3(dots.MinX, dots.MinY, dots.MinZ);  //3
		fwd_up_l	= glm::vec3(dots.MaxX, dots.MaxY, dots.MinZ);  //4
		fwd_up_r	= glm::vec3(dots.MinX, dots.MaxY, dots.MinZ);  //5
		break;

	case LEFT:
		fwd			= glm::vec3(glm::normalize(glm::cross(trans.getForward(), trans.getUp()))); //suppose +Z  1 
		fwd_dwn_l	= glm::vec3(dots.MaxX, dots.MinY, dots.MaxZ);  //2
		fwd_dwn_r	= glm::vec3(dots.MaxX, dots.MinY, dots.MinZ);  //3
		fwd_up_l	= glm::vec3(dots.MaxX, dots.MaxY, dots.MaxZ);  //4
		fwd_up_r	= glm::vec3(dots.MaxX, dots.MaxY, dots.MinZ);  //5
		break;

	case RIGHT:
		fwd			= glm::vec3(glm::normalize(glm::cross(trans.getUp(), trans.getForward()))); //suppose +Z  1 
		fwd_dwn_l	= glm::vec3(dots.MinX, dots.MinY, dots.MaxZ);  //2
		fwd_dwn_r	= glm::vec3(dots.MinX, dots.MinY, dots.MinZ);  //3
		fwd_up_l	= glm::vec3(dots.MinX, dots.MaxY, dots.MaxZ);  //4
		fwd_up_r	= glm::vec3(dots.MinX, dots.MaxY, dots.MinZ);  //5
		break;

	case UP:
		fwd			= glm::vec3(glm::normalize(trans.getUp())); //suppose +Z  1 
		fwd_dwn_l	= glm::vec3(dots.MinX, dots.MaxY, dots.MaxZ);  //2
		fwd_dwn_r	= glm::vec3(dots.MinX, dots.MaxY, dots.MinZ);  //3
		fwd_up_l	= glm::vec3(dots.MaxX, dots.MaxY, dots.MaxZ);  //4
		fwd_up_r	= glm::vec3(dots.MaxX, dots.MaxY, dots.MinZ);  //5
		break;

	case DOWN:
		fwd			= glm::vec3(glm::normalize(-trans.getUp())); //suppose +Z  1 
		fwd_dwn_l	= glm::vec3(dots.MaxX, dots.MinY, dots.MaxZ);  //2
		fwd_dwn_r	= glm::vec3(dots.MaxX, dots.MinY, dots.MinZ);  //3
		fwd_up_l	= glm::vec3(dots.MinX, dots.MinY, dots.MaxZ);  //4
		fwd_up_r	= glm::vec3(dots.MinX, dots.MinY, dots.MinZ);  //5
		break;
	}

	rays.push_back(dbb::line(trans.getModelMatrix() * glm::vec4(*m_center,1.0f), glm::mat3(trans.getModelMatrix()) * glm::normalize(fwd)));
	rays.push_back(dbb::line(trans.getModelMatrix() * glm::vec4(*m_center, 1.0f), glm::mat3(trans.getModelMatrix()) * glm::normalize(fwd_dwn_l - *m_center)));
	rays.push_back(dbb::line(trans.getModelMatrix() * glm::vec4(*m_center, 1.0f), glm::mat3(trans.getModelMatrix()) * glm::normalize(fwd_dwn_r - *m_center)));
	rays.push_back(dbb::line(trans.getModelMatrix() * glm::vec4(*m_center, 1.0f), glm::mat3(trans.getModelMatrix()) * glm::normalize(fwd_up_l - *m_center)));
	rays.push_back(dbb::line(trans.getModelMatrix() * glm::vec4(*m_center, 1.0f), glm::mat3(trans.getModelMatrix()) * glm::normalize(fwd_up_r - *m_center)));

	glm::vec4 center4 = glm::vec4(*m_center, 1.0f);
	getForwardRayLengths(trans, moveDirection, lengths);
	glm::mat4 scal_mat = trans.getScale();

	lengths.push_back(glm::length2(scal_mat * (glm::vec4(fwd_dwn_l, 1.0f) - center4)));
	lengths.push_back(glm::length2(scal_mat * (glm::vec4(fwd_dwn_r, 1.0f) - center4)));
	lengths.push_back(glm::length2(scal_mat * (glm::vec4(fwd_up_l, 1.0f) - center4)));
	lengths.push_back(glm::length2(scal_mat * (glm::vec4(fwd_up_r, 1.0f) - center4)));

	return rays;
}

//-------------------------------------------------------------------------------
void BoxCollider::getForwardRayLengths(const ITransform & trans, Side moveDirection, std::vector<float>& lengths) const
{
	if (lengths.empty())
		lengths.push_back(float());
	glm::vec3 center = *m_center;
	glm::vec4 center4 = glm::vec4(center, 1.0f);
	glm::mat4 scal_mat = trans.getScale();
	switch (moveDirection)
	{
	case FORWARD:
		lengths[0] = glm::length2(scal_mat * (glm::vec4(center.x, center.y, dots.MaxZ, 1.0f) - center4)); // if direction is +Z !!!
		break;
	case BACK:
		lengths[0] = glm::length2(scal_mat *( glm::vec4(center.x, center.y, dots.MinZ, 1.0f) - center4)); // if direction is +Z !!!
		break;
	case RIGHT:
		lengths[0] = glm::length2(scal_mat * (glm::vec4(dots.MaxX, center.y, center.z, 1.0f) - center4)); // if direction is +Z !!!
		break;
	case LEFT:
		lengths[0] = glm::length2(scal_mat *(glm::vec4(dots.MinX, center.y, center.z, 1.0f) - center4)); // if direction is +Z !!!
		break;
	case UP:	
		lengths[0] = glm::length2(scal_mat *(glm::vec4(center.x, dots.MaxY, center.z, 1.0f) - center4)); // if direction is +Z !!!
		break;
	case DOWN:		
		lengths[0] = glm::length2(scal_mat *(glm::vec4(center.x, dots.MinY, center.z, 1.0f) - center4)); // if direction is +Z !!!
		break;
	}
}

//--------------------------------------------------------------------------------------------
bool BoxCollider::CollidesWith(const ITransform & trans1,
															 const ITransform & trans2,
															 const ICollider & other,
															 Side moveDirection, 
															 eCollision& collision)
{
	std::vector<float> lengths;
	std::vector<dbb::line> rays = getRays(trans1, moveDirection, lengths);
	std::vector<glm::mat3> boundings = other.GetBoundingTriangles(trans2);

	glm::vec3 center = trans1.getModelMatrix() * glm::vec4(GetCenter(),1.0f);

	for (int i = 0; i < rays.size(); ++i)
	{
		for (auto& triangle : boundings)
		{
			dbb::plane Plane(triangle);
			glm::vec3 intersaction = dbb::intersection(Plane, rays[i]);
			if (dbb::IsInside(triangle, intersaction) && glm::dot(rays[i].p, glm::vec3(intersaction - rays[i].M)) > 0.0f) // if not behind
				if(glm::length2(intersaction - center) <= lengths[i])
				{
					collision.intersaction  = intersaction;
					collision.triangle		= triangle;
					return true; // Collision found!
				}
		}
	}
	return false;
}

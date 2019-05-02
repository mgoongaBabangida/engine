#include "stdafx.h"
#include "BoxCollider.h"
#include "Structures.h"

void BoxCollider::calculateExtremDots(const std::vector<glm::vec3>& positions)
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
}

std::vector<glm::mat3> BoxCollider::getBoundingTriangles(const Transform& trans)const
{

 // if ((dots.MaxX == 0 && dots.MinX == 0 && dots.MaxY == 0 && dots.MinY == 0 && dots.MaxZ == 0 && dots.MinZ == 0) || dynamic==true)
//	this->calculateExtremDots();

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

std::vector<glm::vec3> BoxCollider::getExtrems(const Transform& trans) const
{
	glm::mat4 transform = trans.getModelMatrix();
	std::vector<glm::vec3> ret;
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MinX, dots.MinY, dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(dots.MinX, dots.MinY, dots.MinZ, 1.0f)));
	return ret;
}

glm::vec3 BoxCollider::getCenter() const
{
	return glm::vec3(dots.MaxX - glm::length(dots.MaxX - dots.MinX) / 2, 
					 dots.MaxY - glm::length(dots.MaxY - dots.MinY) / 2, 
					 dots.MaxZ - glm::length(dots.MaxZ - dots.MinZ) / 2);// always == line.M
}

std::vector<dbb::line> BoxCollider::getRays(const Transform & trans, Side moveDirection, std::vector<float>& lengths) const
{
	glm::vec3 center = getCenter();
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

	/*std::cout << "center= " << center.x << " " << center.y << " " << center.z << std::endl;
	std::cout << "fwd_dwn_l= " << fwd_dwn_l.x << " " << fwd_dwn_l.y << " " << fwd_dwn_l.z << std::endl;
	std::cout << "fwd_dwn_r= " << fwd_dwn_r.x << " " << fwd_dwn_r.y << " " << fwd_dwn_r.z << std::endl;
	std::cout << "fwd_up_l= " << fwd_up_l.x << " " << fwd_up_l.y << " " << fwd_up_l.z << std::endl;
	std::cout << "fwd_up_r= " << fwd_up_r.x << " " << fwd_up_r.y << " " << fwd_up_r.z << std::endl;*/

	rays.push_back(dbb::line(trans.getModelMatrix() * glm::vec4(center,1.0f), glm::mat3(trans.getModelMatrix()) * glm::normalize(fwd)));
	rays.push_back(dbb::line(trans.getModelMatrix() * glm::vec4(center, 1.0f), glm::mat3(trans.getModelMatrix()) * glm::normalize(fwd_dwn_l - center)));
	rays.push_back(dbb::line(trans.getModelMatrix() * glm::vec4(center, 1.0f), glm::mat3(trans.getModelMatrix()) * glm::normalize(fwd_dwn_r - center)));
	rays.push_back(dbb::line(trans.getModelMatrix() * glm::vec4(center, 1.0f), glm::mat3(trans.getModelMatrix()) * glm::normalize(fwd_up_l - center)));
	rays.push_back(dbb::line(trans.getModelMatrix() * glm::vec4(center, 1.0f), glm::mat3(trans.getModelMatrix()) * glm::normalize(fwd_up_r - center)));

	glm::vec4 center4 = glm::vec4(center, 1.0f);
	getForwardRayLengths(trans, moveDirection, lengths);
	glm::mat4 scal_mat = trans.getScale();

	lengths.push_back(glm::length(scal_mat * (glm::vec4(fwd_dwn_l, 1.0f) - center4)));
	lengths.push_back(glm::length(scal_mat *( glm::vec4(fwd_dwn_r, 1.0f) - center4)));
	lengths.push_back(glm::length(scal_mat *( glm::vec4(fwd_up_l, 1.0f) - center4)));
	lengths.push_back(glm::length(scal_mat * (glm::vec4(fwd_up_r, 1.0f) - center4)));

	return rays;
}

void BoxCollider::getForwardRayLengths(const Transform & trans, Side moveDirection, std::vector<float>& lengths) const
{
	if (lengths.empty())
		lengths.push_back(float());
	glm::vec3 center = getCenter();
	glm::vec4 center4 = glm::vec4(center, 1.0f);
	glm::mat4 scal_mat = trans.getScale();
	switch (moveDirection) {
	case FORWARD:
		lengths[0] = glm::length(scal_mat * (glm::vec4(center.x, center.y, dots.MaxZ, 1.0f) - center4)); // if direction is +Z !!!
		break;
	case BACK:
		lengths[0] = glm::length(scal_mat *( glm::vec4(center.x, center.y, dots.MinZ, 1.0f) - center4)); // if direction is +Z !!!
		break;
	case RIGHT:
		lengths[0] = glm::length(scal_mat * (glm::vec4(dots.MaxX, center.y, center.z, 1.0f) - center4)); // if direction is +Z !!!
		break;
	case LEFT:
		lengths[0] = glm::length(scal_mat *(glm::vec4(dots.MinX, center.y, center.z, 1.0f) - center4)); // if direction is +Z !!!
		break;
	case UP:	
		lengths[0] = glm::length(scal_mat *(glm::vec4(center.x, dots.MaxY, center.z, 1.0f) - center4)); // if direction is +Z !!!
		break;
	case DOWN:		
		lengths[0] = glm::length(scal_mat *(glm::vec4(center.x, dots.MinY, center.z, 1.0f) - center4)); // if direction is +Z !!!
		break;
	}
}

bool BoxCollider::collidesWith(const Transform & trans1, 
							   const Transform & trans2, 
							   const BoxCollider & other, 
							   Side moveDirection, 
							   eCollision& collision)
{
	std::vector<float> lengths;
	std::vector<dbb::line> rays = getRays(trans1, moveDirection, lengths);
	std::vector<glm::mat3> boundings = other.getBoundingTriangles(trans2);

	glm::vec3 center = trans1.getModelMatrix() * glm::vec4(getCenter(),1.0f);

	for (int i = 0; i < rays.size(); ++i)
	{
		for (auto& triangle : boundings)
		{
			dbb::plane Plane(triangle);
			glm::vec3 intersaction = dbb::intersection(Plane, rays[i]);
			if (dbb::IsInside(triangle, intersaction) && glm::dot(rays[i].p, glm::vec3(intersaction - rays[i].M)) > 0.0f) // if not behind
				if(glm::length(intersaction - center) <= lengths[i])
				{
					collision.intersaction  = intersaction;
					collision.triangle		= triangle;
					return true; // Collision found!
				}
		}
	}
	return false;
}

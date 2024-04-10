#include "stdafx.h"
#include "BoxCollider.h"

#include <glm\glm\gtx\norm.hpp>

//-------------------------------------------------------------------------------
void BoxCollider::CalculateExtremDots(const eObject* _object)
{
	for (int k = 0; k < _object->GetModel()->GetMeshCount(); ++k)
	{
		auto vertices = _object->GetModel()->Get3DMeshes()[k]->GetVertexs();
		for (int i = 0; i < vertices.size(); ++i)
		{
			if (vertices[i].Position.x > m_dots.MaxX)
				m_dots.MaxX = vertices[i].Position.x;
			if (vertices[i].Position.x < m_dots.MinX)
				m_dots.MinX = vertices[i].Position.x;
			if (vertices[i].Position.y > m_dots.MaxY)
				m_dots.MaxY = vertices[i].Position.y;
			if (vertices[i].Position.y < m_dots.MinY)
				m_dots.MinY = vertices[i].Position.y;
			if (vertices[i].Position.z > m_dots.MaxZ)
				m_dots.MaxZ = vertices[i].Position.z;
			if (vertices[i].Position.z < m_dots.MinZ)
				m_dots.MinZ = vertices[i].Position.z;
		}
	}
	m_center = GetCenter();
	m_radius = GetRadius();
	m_model_name = _object->GetModel()->GetName();
}

//-------------------------------------------------------------------------------
std::vector<glm::mat3> BoxCollider::GetBoundingTriangles(const ITransform& trans) const
{
	//extrem dots should exist
	glm::mat4 transform = trans.getModelMatrix();
	std::vector<glm::mat3> ret; // Getting 12 triangles of the bouning cube

	ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform * glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ, 1.0f)),
													glm::vec3(transform * glm::vec4(m_dots.MaxX, m_dots.MaxY, m_dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MinY, m_dots.MaxZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MinY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MinY, m_dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MinY, m_dots.MaxZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MaxY, m_dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MinY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MinZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MaxY, m_dots.MaxZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MinY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MinY, m_dots.MaxZ, 1.0f))));

	ret.push_back(glm::mat3(glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MinZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MinY, m_dots.MaxZ, 1.0f)),
													glm::vec3(transform *glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ, 1.0f))));

	return ret;
}

//-------------------------------------------------------------------------------
std::vector<glm::vec3> BoxCollider::GetExtrems(const ITransform& trans) const
{
	glm::mat4 transform = trans.getModelMatrix();
	std::vector<glm::vec3> ret;
	ret.push_back(glm::vec3(transform * glm::vec4(m_dots.MaxX, m_dots.MaxY, m_dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(m_dots.MinX, m_dots.MinY, m_dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(transform * glm::vec4(m_dots.MinX, m_dots.MinY, m_dots.MaxZ, 1.0f)));
	return ret;
}

//-------------------------------------------------------------------------------
std::vector<glm::mat3> BoxCollider::GetBoundingTrianglesLocalSpace() const
{
	//extrem dots should exist
	std::vector<glm::mat3> ret; // Getting 12 triangles of the bouning cube
	ret.push_back(glm::mat3(glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ),
													glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ),
													glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ),
													glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MinZ),
													glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MinZ),
													glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MaxZ),
													glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MinZ),
													glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MinZ),
													glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ),
													glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MaxZ),
													glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ),
													glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ),
													glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MinZ),
													glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MinZ),
													glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MinZ)));

	ret.push_back(glm::mat3(glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MinZ),
													glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MinZ),
													glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ)));

	ret.push_back(glm::mat3(glm::vec3(m_dots.MaxX, m_dots.MaxY,m_dots.MinZ),
													glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MinZ),
													glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ),
													glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ),
													glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MinZ),
													glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MinZ),
													glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MaxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MinZ),
													glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MaxZ),
													glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ)));
	return ret;
}

//-------------------------------------------------------------------------------
std::vector<glm::vec3> BoxCollider::GetExtremsLocalSpace() const
{
	std::vector<glm::vec3> ret;
	ret.push_back(glm::vec3(glm::vec4(m_dots.MaxX, m_dots.MaxY, m_dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(m_dots.MaxX, m_dots.MinY, m_dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(m_dots.MinX, m_dots.MinY, m_dots.MinZ, 1.0f)));
	ret.push_back(glm::vec3(glm::vec4(m_dots.MinX, m_dots.MinY, m_dots.MaxZ, 1.0f)));
	return ret;
}

//-------------------------------------------------------------------------------
glm::vec3 BoxCollider::GetCenter() const
{
	if (m_center == std::nullopt)
	{
		m_center = glm::vec3(m_dots.MaxX - glm::length(m_dots.MaxX - m_dots.MinX) / 2,
												 m_dots.MaxY - glm::length(m_dots.MaxY - m_dots.MinY) / 2,
												 m_dots.MaxZ - glm::length(m_dots.MaxZ - m_dots.MinZ) / 2);// always == line.M
	}
	return *m_center;
}

//-------------------------------------------------------------------------------
float BoxCollider::GetRadius() const
{
	if (m_radius == 0.0f)
	{
		glm::vec3 corner = glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MaxZ);
		m_radius = glm::length(corner - *m_center);
	}
	return m_radius;
}

//-------------------------------------------------------------------------------
std::vector<dbb::line> BoxCollider::_getRays(const ITransform & trans, Side moveDirection, std::vector<float>& lengths)
{
	std::vector<dbb::line> rays;
	glm::vec3 fwd, fwd_dwn_l, fwd_dwn_r, fwd_up_l, fwd_up_r;
	auto model_matrix = trans.getModelMatrix();
	switch (moveDirection) 
	{
	case FORWARD:
		fwd			=		trans.getForward(); //suppose +Z  1 
		fwd_dwn_l	= glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ);  //2
		fwd_dwn_r	= glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MaxZ);  //3
		fwd_up_l	= glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MaxZ);  //4
		fwd_up_r	= glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ);  //5
		break;

	case BACK:
		fwd			=		-trans.getForward(); //suppose +Z  1 
		fwd_dwn_l	= glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MinZ);  //2
		fwd_dwn_r	= glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MinZ);  //3
		fwd_up_l	= glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ);  //4
		fwd_up_r	= glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MinZ);  //5
		break;

	case LEFT:
		fwd			=		glm::vec3(glm::cross(trans.getUp(), trans.getForward())); //suppose +Z  1 
		fwd_dwn_l	= glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ);  //2
		fwd_dwn_r	= glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MinZ);  //3
		fwd_up_l	= glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MaxZ);  //4
		fwd_up_r	= glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ);  //5
		break;

	case RIGHT:
		fwd			=		glm::vec3(glm::cross(trans.getForward(), trans.getUp())); //suppose +Z  1 
		fwd_dwn_l	= glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MaxZ);  //2
		fwd_dwn_r	= glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MinZ);  //3
		fwd_up_l	= glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ);  //4
		fwd_up_r	= glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MinZ);  //5
		break;

	case UP:
		fwd			=		glm::vec3(trans.getUp()); //suppose +Z  1 
		fwd_dwn_l	= glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MaxZ);  //2
		fwd_dwn_r	= glm::vec3(m_dots.MinX, m_dots.MaxY, m_dots.MinZ);  //3
		fwd_up_l	= glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MaxZ);  //4
		fwd_up_r	= glm::vec3(m_dots.MaxX, m_dots.MaxY, m_dots.MinZ);  //5
		break;

	case DOWN:
		fwd			=		glm::vec3(-trans.getUp()); //suppose +Z  1 
		fwd_dwn_l	= glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MaxZ);  //2
		fwd_dwn_r	= glm::vec3(m_dots.MaxX, m_dots.MinY, m_dots.MinZ);  //3
		fwd_up_l	= glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MaxZ);  //4
		fwd_up_r	= glm::vec3(m_dots.MinX, m_dots.MinY, m_dots.MinZ);  //5
		break;
	}

	rays.push_back(dbb::line(model_matrix * glm::vec4(*m_center,1.0f),  glm::mat3(model_matrix) * (fwd)));
	rays.push_back(dbb::line(model_matrix * glm::vec4(*m_center, 1.0f), glm::mat3(model_matrix) * (fwd_dwn_l - *m_center)));
	rays.push_back(dbb::line(model_matrix * glm::vec4(*m_center, 1.0f), glm::mat3(model_matrix) * (fwd_dwn_r - *m_center)));
	rays.push_back(dbb::line(model_matrix * glm::vec4(*m_center, 1.0f), glm::mat3(model_matrix) * (fwd_up_l - *m_center)));
	rays.push_back(dbb::line(model_matrix * glm::vec4(*m_center, 1.0f), glm::mat3(model_matrix) * (fwd_up_r - *m_center)));

	glm::vec4 center4 = glm::vec4(*m_center, 1.0f);
	_getForwardRayLengths(trans, moveDirection, lengths);
	glm::mat4 scal_mat = trans.getScale();

	lengths.push_back(glm::length2(scal_mat * (glm::vec4(fwd_dwn_l, 1.0f) - center4)));
	lengths.push_back(glm::length2(scal_mat * (glm::vec4(fwd_dwn_r, 1.0f) - center4)));
	lengths.push_back(glm::length2(scal_mat * (glm::vec4(fwd_up_l, 1.0f) - center4)));
	lengths.push_back(glm::length2(scal_mat * (glm::vec4(fwd_up_r, 1.0f) - center4)));

	return rays;
}

//-------------------------------------------------------------------------------
void BoxCollider::_getForwardRayLengths(const ITransform & trans, Side moveDirection, std::vector<float>& lengths) const
{
		if (lengths.empty())
			lengths.push_back(float());
		glm::vec3 center = *m_center; //@todo optional check ?
		glm::vec4 center4 = glm::vec4(*m_center, 1.0f);
		glm::mat4 scal_mat = trans.getScale();
		switch (moveDirection)
		{
		case FORWARD:
			lengths[0] = glm::length2(scal_mat * (glm::vec4(center.x, center.y, m_dots.MaxZ, 1.0f) - center4)); // if direction is +Z !!!
			break;
		case BACK:
			lengths[0] = glm::length2(scal_mat * (glm::vec4(center.x, center.y, m_dots.MinZ, 1.0f) - center4)); // if direction is +Z !!!
			break;
		case RIGHT:
			lengths[0] = glm::length2(scal_mat * (glm::vec4(m_dots.MaxX, center.y, center.z, 1.0f) - center4)); // if direction is +Z !!!
			break;
		case LEFT:
			lengths[0] = glm::length2(scal_mat * (glm::vec4(m_dots.MinX, center.y, center.z, 1.0f) - center4)); // if direction is +Z !!!
			break;
		case UP:
			lengths[0] = glm::length2(scal_mat * (glm::vec4(center.x, m_dots.MaxY, center.z, 1.0f) - center4)); // if direction is +Z !!!
			break;
		case DOWN:
			lengths[0] = glm::length2(scal_mat * (glm::vec4(center.x, m_dots.MinY, center.z, 1.0f) - center4)); // if direction is +Z !!!
			break;
		}
}

//--------------------------------------------------------------------------------------------
bool BoxCollider::CollidesWith(const ITransform& _trans,
	const ITransform& _trans_other,
	const ICollider& _other,
	Side _moveDirection,
	eCollision& _collision)
{
	std::vector<float> lengths;
	std::vector<dbb::line> rays = _getRays(_trans, _moveDirection, lengths);
	glm::vec3 center = _trans.getModelMatrix() * glm::vec4(GetCenter(), 1.0f);

	std::vector<glm::mat3> boundings = _other.GetBoundingTriangles(_trans_other);
	for (int i = 0; i < rays.size(); ++i)
	{
		for (auto& triangle : boundings)
		{
			dbb::plane Plane(triangle);
			glm::vec3 intersaction = dbb::intersection(Plane, rays[i]);
			if (dbb::IsInside(triangle, intersaction)
				&& glm::dot(rays[i].p, glm::vec3(intersaction - rays[i].M)) > 0.0f) // if not behind
				if (glm::length2(intersaction - center) <= lengths[i])
				{
					_collision.intersaction = intersaction;
					_collision.triangle = triangle;
					return true; // Collision found!
				}
		}
	}
	return false;
}

//-------------------------------------------------------------------
bool BoxCollider::CollidesWith(const ITransform& _trans, std::vector<shObject> _objects, Side _moveDirection, eCollision& _collision)
{
	std::vector<shObject> objects_to_check = _objects;
	if (m_check_sphere_overlap)
	{
		for (auto& obj : _objects)
		{
			if (obj->GetCollider() == this || !obj->GetCollider())
				continue;

			if (_CheckByRadius(_trans, *obj->GetTransform(), obj->GetCollider()))
				objects_to_check.push_back(obj);
		}
		if (objects_to_check.empty())
			return false;
	}

	std::vector<float> lengths;
	std::vector<dbb::line> rays = _getRays(_trans, _moveDirection, lengths);
	glm::vec3 center = _trans.getModelMatrix() * glm::vec4(GetCenter(), 1.0f);

	for (auto& obj : objects_to_check)
	{
		if (obj->GetCollider() == this || obj->GetCollider() == nullptr)
			continue;

		const std::vector<glm::mat3> boundings = obj->GetCollider()->GetBoundingTriangles(*obj->GetTransform());
		for (int i = 0; i < rays.size(); ++i)
		{
			for (int j = 0; j < boundings.size(); ++j)
			{
				dbb::plane Plane(boundings[j]);
				glm::vec3 intersaction = dbb::intersection(Plane, rays[i]);
				if (dbb::IsInside(boundings[j], intersaction))
				{
					if (glm::length2(intersaction - center) <= lengths[i])
					{
						if (glm::dot(rays[i].p, glm::vec3(intersaction - rays[i].M)) > 0.0f)// if not behind
						{
							_collision.intersaction = intersaction;
							_collision.triangle = boundings[j];
							_collision.collidee = obj.get();
							return true; // Collision found!
						}
					}
				}
			}
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool BoxCollider::IsInsideOfAABB(const ITransform& _trans, const ITransform& _trans2, const ICollider& _other)
{
	std::vector<glm::vec3> extremsWS = GetExtremsLocalSpace();
	for (auto& extrem : extremsWS)
		extrem = _trans.getModelMatrix() * glm::vec4(extrem, 1.0f);

	std::vector<glm::vec3> extresWSOther = _other.GetExtremsLocalSpace();
	for (auto& extrem : extresWSOther)
		extrem = _trans2.getModelMatrix() * glm::vec4(extrem, 1.0f);

	extremDots otherWorldSpaceAABB;
	for (const auto& extrem : extresWSOther)
	{
		if (extrem.x > otherWorldSpaceAABB.MaxX)
			otherWorldSpaceAABB.MaxX = extrem.x;
		if (extrem.x < otherWorldSpaceAABB.MinX)
			otherWorldSpaceAABB.MinX = extrem.x;
		if (extrem.y > otherWorldSpaceAABB.MaxY)
			otherWorldSpaceAABB.MaxY = extrem.y;
		if (extrem.y < otherWorldSpaceAABB.MinY)
			otherWorldSpaceAABB.MinY = extrem.y;
		if (extrem.z > otherWorldSpaceAABB.MaxZ)
			otherWorldSpaceAABB.MaxZ = extrem.z;
		if (extrem.z < otherWorldSpaceAABB.MinZ)
			otherWorldSpaceAABB.MinZ = extrem.z;
	}

	for (const auto& extremDot : extremsWS)
	{
		if (extremDot.x > otherWorldSpaceAABB.MaxX ||
			  extremDot.x < otherWorldSpaceAABB.MinX ||
			  extremDot.y > otherWorldSpaceAABB.MaxY ||
			  extremDot.y < otherWorldSpaceAABB.MinY ||
			  extremDot.z > otherWorldSpaceAABB.MaxZ ||
			  extremDot.z < otherWorldSpaceAABB.MinZ)
			return false; // this IS NOT totally inside AABB worldPos of other
	}
	return true; // this IS totally inside AABB worldPos of other
}

//----------------------------------------------------------------------------------------
dbb::OBB BoxCollider::_GetOBB(const ITransform& _trans) const
{
	dbb::OBB obb;
	obb.origin = _trans.getModelMatrix() * glm::vec4(this->GetCenter(), 1.0f);
	obb.size = { ((this->GetExtremDotsLocalSpace().MaxX - this->GetExtremDotsLocalSpace().MinX) * _trans.getScaleAsVector().x )/ 2,
							 ((this->GetExtremDotsLocalSpace().MaxY - this->GetExtremDotsLocalSpace().MinY) * _trans.getScaleAsVector().y) / 2,
							 ((this->GetExtremDotsLocalSpace().MaxZ - this->GetExtremDotsLocalSpace().MinZ) * _trans.getScaleAsVector().z) / 2 };
	obb.orientation = glm::toMat4(_trans.getRotation());
	if (obb.size.y == 0.0f) // for planes to have some volume @todo make collider for planes ?
		obb.size.y = 0.3f;
	return obb;
}

//----------------------------------------------------------------------------------------
dbb::sphere BoxCollider::_GetSphere(const ITransform& _trans) const
{
	dbb::sphere sphere;
	sphere.position = _trans.getTranslation();
	sphere.radius = ((this->GetExtremDotsLocalSpace().MaxX - this->GetExtremDotsLocalSpace().MinX) * _trans.getScaleAsVector().x) / 2;
	return sphere;
}

//----------------------------------------------------------------------------------------
bool BoxCollider::_CheckByRadius(const ITransform& _trans, const ITransform& _trans_other, ICollider* _other)
{
	glm::vec4 center				= _trans.getModelMatrix()				* glm::vec4(GetCenter(), 1.0f);
	glm::vec4 other_center	= _trans_other.getModelMatrix() * glm::vec4(_other->GetCenter(), 1.0f);

	float distance_betwen_centers = glm::length(center - other_center);
	float two_radius_added = (GetRadius() * _trans.getScaleAsVector().x) + (_other->GetRadius() * _trans_other.getScaleAsVector().x);
	if (distance_betwen_centers > two_radius_added)
		return false; //too far to collide
	else
		return true; // can collide
}

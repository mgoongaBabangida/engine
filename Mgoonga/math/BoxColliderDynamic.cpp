#include "BoxColliderDynamic.h"

//------------------------------------------------------------------
void BoxColliderDynamic::CalculateExtremDots(const eObject* _object)
{
  BoxCollider::CalculateExtremDots(_object);

  m_rigger = _object->GetRigger();
	if (m_rigger && m_data.empty())
	{
		for (size_t i = 0; i < m_rigger->GetAnimationCount(); ++i)
		{
			m_data.push_back({ m_rigger->GetAnimationNames()[i] });
			std::vector<glm::mat4> gBones; size_t frame_index = 0;
			while(true)
			{
				gBones = m_rigger->GetMatrices(m_data.back().name, frame_index++);
				if (gBones.empty())
					break;

				//calculate extrem dots for frmae j
				extremDots dts;
				for (int k = 0; k < _object->GetModel()->GetMeshCount(); ++k)
				{
					auto vertices = _object->GetModel()->Get3DMeshes()[k]->GetVertexs();
					for (int i = 0; i < vertices.size(); ++i)
					{
						glm::mat4 BoneTransform = gBones[vertices[i].boneIDs[0]] * vertices[i].weights[0];
											BoneTransform += gBones[vertices[i].boneIDs[1]] * vertices[i].weights[1];
											BoneTransform += gBones[vertices[i].boneIDs[2]] * vertices[i].weights[2];
											BoneTransform += gBones[vertices[i].boneIDs[3]] * vertices[i].weights[3];

						glm::vec4 vertexPos = BoneTransform * glm::vec4(vertices[i].Position, 1.0);

						if (vertexPos.x > dts.MaxX)
							dts.MaxX = vertexPos.x;
						if (vertexPos.x < dts.MinX)
							dts.MinX = vertexPos.x;
						if (vertexPos.y > dts.MaxY)
							dts.MaxY = vertexPos.y;
						if (vertexPos.y < dts.MinY)
							dts.MinY = vertexPos.y;
						if (vertexPos.z > dts.MaxZ)
							dts.MaxZ = vertexPos.z;
						if (vertexPos.z < dts.MinZ)
							dts.MinZ = vertexPos.z;
					}
				}
				m_data.back().extremDots.push_back(dts);
				glm::vec3 center = glm::vec3(dts.MaxX - glm::length(dts.MaxX - dts.MinX) / 2,
																		 dts.MaxY - glm::length(dts.MaxY - dts.MinY) / 2,
																		 dts.MaxZ - glm::length(dts.MaxZ - dts.MinZ) / 2);
				m_data.back().centers.push_back(center);

				glm::vec3 corner = glm::vec3(dts.MaxX, dts.MaxY, dts.MaxZ);
				m_data.back().radiuses.push_back(glm::length(corner - center));
			}
		}
	}
}

//------------------------------------------------------------------
std::vector<glm::mat3> BoxColliderDynamic::GetBoundingTriangles(const ITransform& _trans) const
{
	if (!m_rigger || !m_rigger->GetCurrentAnimation())
		return BoxCollider::GetBoundingTriangles(_trans);
	else
	{
		auto it = std::find_if(m_data.begin(), m_data.end(), [this](const AnimationData& _data) { return _data.name == m_rigger->GetCurrentAnimationName(); });
		if (it != m_data.end())
		{
			size_t frame = m_rigger->GetCurrentAnimationFrameIndex();
			if (frame == -1)
				return BoxCollider::GetBoundingTriangles(_trans);
			else
			{
				auto dots = it->extremDots[frame];
				glm::mat4 transform = _trans.getModelMatrix();
				std::vector<glm::mat3> ret; // Getting 12 triangles of the bouning cube
				ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MaxZ, 1.0f))));

				ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f))));

				ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MinX, dots.MinY, dots.MaxZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MaxZ, 1.0f))));

				ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MinX, dots.MinY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MinX, dots.MinY, dots.MaxZ, 1.0f))));

				ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MinX, dots.MinY, dots.MaxZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MaxZ, 1.0f))));

				ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MaxZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MaxZ, 1.0f))));

				ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MinX, dots.MinY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f))));

				ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f))));

				ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MaxZ, 1.0f))));

				ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MaxX, dots.MinY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MaxX, dots.MaxY, dots.MaxZ, 1.0f))));

				ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MinX, dots.MinY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MinX, dots.MinY, dots.MaxZ, 1.0f))));

				ret.push_back(glm::mat3(glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MinX, dots.MinY, dots.MinZ, 1.0f)),
					glm::vec3(transform * glm::vec4(dots.MinX, dots.MaxY, dots.MaxZ, 1.0f))));
				return ret;
			}
		}
		else
			return BoxCollider::GetBoundingTriangles(_trans);
	}
}

//------------------------------------------------------------------
std::vector<glm::vec3> BoxColliderDynamic::GetExtrems(const ITransform& _trans) const
{
	if (!m_rigger || !m_rigger->GetCurrentAnimation())
		return BoxCollider::GetExtrems(_trans);
	else
	{
		auto it = std::find_if(m_data.begin(), m_data.end(), [this](const AnimationData& _data) { return _data.name == m_rigger->GetCurrentAnimationName(); });
		if (it != m_data.end())
		{
			size_t frame = m_rigger->GetCurrentAnimationFrameIndex();
			if(frame == -1)
				return BoxCollider::GetExtrems(_trans);
			else
			{
				auto dots = it->extremDots[frame];
				glm::mat4 transform = _trans.getModelMatrix();
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
		}
		else
			return BoxCollider::GetExtrems(_trans);
	}
}

//------------------------------------------------------------------
glm::vec3 BoxColliderDynamic::GetCenter()
{
	if (!m_rigger || m_rigger->GetCurrentAnimationName() == std::string{})
		return BoxCollider::GetCenter();
	else
	{
		auto it = std::find_if(m_data.begin(), m_data.end(), [this](const AnimationData& _data) { return _data.name == m_rigger->GetCurrentAnimationName(); });
		if (it != m_data.end())
		{
			return it->centers[m_rigger->GetCurrentAnimationFrameIndex()];
		}
		else
			return BoxCollider::GetCenter();
	}
}

//------------------------------------------------------------------
float BoxColliderDynamic::GetRadius()
{
	if (!m_rigger || m_rigger->GetCurrentAnimationName() == std::string{})
		return 0.0f;
	else
	{
		auto it = std::find_if(m_data.begin(), m_data.end(), [this](const AnimationData& _data) { return _data.name == m_rigger->GetCurrentAnimationName(); });
		if (it != m_data.end())
		{
			return it->radiuses[m_rigger->GetCurrentAnimationFrameIndex()];
		}
		else
			return BoxCollider::GetRadius();
	}
}

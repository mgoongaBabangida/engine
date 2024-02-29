#include "RigidBodyDBB.h"

namespace dbb
{
  glm::vec3 RigidBody::g_gravity = glm::vec3(0.0f, -9.82f, 0.0f);
  float RigidBody::g_friction = 0.95f;

  //--------------------------------------------
  void RigidBody::Update(float _deltaTime) //needs to be called at fixed fps
  {
    glm::vec3 velocity = m_position - m_oldPosition;
    m_oldPosition = m_position;
    float deltaSquare = _deltaTime * _deltaTime;
    m_position = m_position + (velocity * g_friction + m_forces * deltaSquare);
  }

  //---------------------------------------------
  void RigidBody::ApplyForces()
  {
    m_forces = g_gravity;
  }

  //---------------------------------------------------------------------
  void RigidBody::SolveConstraints(const std::vector<OBB>& _constraints)
  {
    int size = _constraints.size();
    for (int i = 0; i < size; ++i)
    {
      dbb::lineSegment traveled(m_oldPosition, m_position);
      if (traveled.LineTest(_constraints[i]))
      {
        glm::vec3 velocity = m_position - m_oldPosition;
        glm::vec3 direction = glm::normalize(velocity);
        dbb::ray ray(m_oldPosition, direction);
        RaycastResult result;
        if (ray.Raycast(_constraints[i], result))
        {
          m_position = result.point + result.normal * 0.003f;
          glm::vec3 vn = result.normal * glm::dot(result.normal, velocity);
          glm::vec3 vt = velocity - vn;
          m_oldPosition = m_position - (vt - vn * m_bounce);
          break;
        }
      }
    }
  }
}
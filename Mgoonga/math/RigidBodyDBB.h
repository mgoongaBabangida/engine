#pragma once

#include "math.h"
#include "Geometry.h"

#include <vector>

namespace dbb
{
  //---------------------------------------------------------------------
  class RigidBody
  {
  public:
    static glm::vec3 g_gravity; //@todo constexpr somwhere above
    static float g_friction;

    virtual void Update(float _deltaTime);
    virtual void ApplyForces();
    virtual void SolveConstraints(const std::vector<OBB>& _constraints);

    void SetPosition(const glm::vec3& pos) { m_position = m_oldPosition = pos; }
    glm::vec3 GetPosition() { return m_position; }
    void SetBounce(float b) { m_bounce = b; }
    float GetBounce() { return m_bounce; }
    glm::vec3 GetVelocity() const { return m_oldPosition - m_position; }

  protected:
    glm::vec3 m_position;
    glm::vec3 m_oldPosition;
    glm::vec3 m_forces;
    float m_mass = 1.0f;
    float m_bounce = 0.7f;
  };
}
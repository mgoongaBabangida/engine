#include "stdafx.h"

#include"Spring.h"

namespace dbb
{
  //------------------------------------------------------
  Particle* dbb::Spring::GetP1()
  {
    return p1;
  }

  //------------------------------------------------------
  Particle* Spring::GetP2()
  {
    return p2;
  }

  //------------------------------------------------------
  void Spring::SetParticles(Particle* _p1, Particle* _p2)
  {
    if (_p1->HasVolume() || _p2->HasVolume())
      return; // it is not a particle

    p1 = _p1;
    p2 = _p2;
  }

  //------------------------------------------------------
  void Spring::SetConstants(float _k, float _b)
  {
    k = _k;
    b = _b;
  }

  //------------------------------------------------------
  void Spring::ApplyForce(float dt)
  {
    // Hooke's Law
    glm::vec3 relPos = p2->GetPosition() - p1->GetPosition();
    glm::vec3 relVel = p2->GetVelocity() - p1->GetVelocity();
    float x = glm::length(relPos) - restingLength;
    float v = glm::length(relVel);
    float F = (-k * x) + (-b * v);
    glm::vec3 impulse = glm::normalize(relPos) * F;
    p1->AddLinearImpulse(impulse * p1->InvMass());
    p2->AddLinearImpulse(impulse * -1.0f * p2->InvMass());
  }
}

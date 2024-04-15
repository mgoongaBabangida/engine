#include "stdafx.h"

#include "Cloth.h"

namespace dbb
{
  //------------------------------------------------------------------------------
  void Cloth::Initialize(int gridSize, float distance, const glm::vec3& position)
  {
    float k = -1.0f;
    float b = 0.0f;
    clothSize = gridSize;

    verts.clear();
    structural.clear();
    shear.clear();
    bend.clear();
    verts.resize(gridSize * gridSize);
    float hs = (float)(gridSize - 1) * 0.5f;
    if (gridSize < 3) {
      gridSize = 3;
    }
    for (int x = 0; x < gridSize; ++x) {
      for (int z = 0; z < gridSize; ++z) {
        int i = z * gridSize + x;
        float x_pos = ((float)x + position.x
          - hs) * distance;
        float z_pos = ((float)z + position.z
          - hs) * distance;
        verts[i].SetPosition(
          glm::vec3(x_pos, position.y, z_pos)
        );
        verts[i].SetMass(1.0f);
        verts[i].SetBounce(0.0f);
        verts[i].SetFriction(0.9f);
      }
    }
    for (int x = 0; x < gridSize; ++x) {
      for (int z = 0; z < gridSize - 1; ++z) {
        int i = z * gridSize + x;
        int j = (z + 1) * gridSize + x;
        glm::vec3 iPos = verts[i].GetPosition();
        glm::vec3 jPos = verts[j].GetPosition();
        float rest = glm::length(iPos - jPos);

        Spring spring(k, b, rest);
        spring.SetParticles(&verts[i], &verts[j]);
        structural.push_back(spring);
      }
    }
    for (int x = 0; x < gridSize - 1; ++x) {
      for (int z = 0; z < gridSize; ++z) {
        int i = z * gridSize + x;
        int j = z * gridSize + (x + 1);
        glm::vec3 iPos = verts[i].GetPosition();
        glm::vec3 jPos = verts[j].GetPosition();
        float rest = glm::length(iPos - jPos);
        Spring spring(k, b, rest);
        spring.SetParticles(&verts[i], &verts[j]);
        structural.push_back(spring);
      }
    }
    for (int x = 0; x < gridSize - 1; ++x) {
      for (int z = 0; z < gridSize - 1; ++z) {
        int i = z * gridSize + x;
        int j = (z + 1) * gridSize + (x + 1);
        glm::vec3 iPos = verts[i].GetPosition();
        glm::vec3 jPos = verts[j].GetPosition();
        float rest = glm::length(iPos - jPos);

        Spring spring(k, b, rest);
        spring.SetParticles(&verts[i], &verts[j]);
        shear.push_back(spring);
      }
    }
    for (int x = 1; x < gridSize; ++x) {
      for (int z = 0; z < gridSize - 1; ++z) {
        int i = z * gridSize + x;
        int j = (z + 1) * gridSize + (x - 1);
        glm::vec3 iPos = verts[i].GetPosition();
        glm::vec3 jPos = verts[j].GetPosition();
        float rest = glm::length(iPos - jPos);
        Spring spring(k, b, rest);
        spring.SetParticles(&verts[i], &verts[j]);
        shear.push_back(spring);
      }
    }
    for (int x = 0; x < gridSize; ++x) {
      for (int z = 0; z < gridSize - 2; ++z) {
        int i = z * gridSize + x;
        int j = (z + 2) * gridSize + x;
        glm::vec3 iPos = verts[i].GetPosition();
        glm::vec3 jPos = verts[j].GetPosition();
        float rest = glm::length(iPos - jPos);
        Spring spring(k, b, rest);
        spring.SetParticles(&verts[i], &verts[j]);
        bend.push_back(spring);
      }
    }

    for (int x = 0; x < gridSize - 2; ++x) {
      for (int z = 0; z < gridSize; ++z) {

        int i = z * gridSize + x;
        int j = z * gridSize + (x + 2);
        glm::vec3 iPos = verts[i].GetPosition();
        glm::vec3 jPos = verts[j].GetPosition();
        float rest = glm::length(iPos - jPos);
        Spring spring(k, b, rest);
        spring.SetParticles(&verts[i], &verts[j]);
        bend.push_back(spring);
      }
    }
  }

  //------------------------------------------------------------------------------
  void Cloth::SetStructuralSprings(float k, float b)
  {
    for (int i = 0; i < structural.size(); ++i) {
      structural[i].SetConstants(k, b);
    }
  }

  //------------------------------------------------------------------------------
  void Cloth::SetShearSprings(float k, float b)
  {
    for (int i = 0, size = shear.size(); i < size; ++i) {
      shear[i].SetConstants(k, b);
    }
  }

  //------------------------------------------------------------------------------
  void Cloth::SetBendSprings(float k, float b)
  {
    for (int i = 0, size = bend.size(); i < size; ++i) {
      bend[i].SetConstants(k, b);
    }
  }
  
  //------------------------------------------------------------------------------
  void Cloth::SetParticleMass(float mass)
  {
    for (int i = 0, size = verts.size(); i < size; ++i) {
      verts[i].SetMass(mass);
    }
  }

  //------------------------------------------------------------------------------
  void Cloth::ApplyForces()
  {
    for (int i = 0, size = verts.size(); i < size; ++i) {
      verts[i].ApplyForces();
    }
  }

  //------------------------------------------------------------------------------
  void Cloth::Update(float dt)
  {
    for (int i = 0, size = verts.size(); i < size; ++i) {
      verts[i].Update(dt);
    }
  }

  //------------------------------------------------------------------------------
  void Cloth::ApplySpringForces(float dt)
  {
    for (int i = 0; i < structural.size(); ++i) {
      structural[i].ApplyForce(dt);
    }
    for (int i = 0, size = shear.size(); i < size; ++i) {
      shear[i].ApplyForce(dt);
    }
    for (int i = 0, size = bend.size(); i < size; ++i) {
      bend[i].ApplyForce(dt);
    }
  }

  //------------------------------------------------------------------------------
  void Cloth::SolveConstraints(const std::vector<OBB>& constraints)
  {
    for (int i = 0, size = verts.size(); i < size; ++i)
      verts[i].SolveConstraints(constraints);
  }

}
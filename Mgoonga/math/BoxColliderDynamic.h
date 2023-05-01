#pragma once
#include "BoxCollider.h"


class DLL_MATH BoxColliderDynamic : public BoxCollider
{
public:
  struct AnimationData
  {
    std::string name;
    std::vector<extremDots> extremDots;
    std::vector <glm::vec3> centers;
  };
  virtual void CalculateExtremDots(const eObject* _object) override;

  virtual std::vector<glm::mat3>	GetBoundingTriangles(const ITransform& trans)const override;
  virtual std::vector<glm::vec3>	GetExtrems(const ITransform& trans) const override;
  virtual glm::vec3 GetCenter() override;

protected:
  std::vector<AnimationData> m_data;
  IRigger*                   m_rigger = nullptr;
};

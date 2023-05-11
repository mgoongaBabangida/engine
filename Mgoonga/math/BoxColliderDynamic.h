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
    std::vector <float>     radiuses;
  };

  BoxColliderDynamic() :BoxCollider() {}

  BoxColliderDynamic(const std::string& _modelName,
                     extremDots _dots,
                     glm::vec3 _center,
                     float _radius,
                     std::vector<AnimationData> _data,
                     const std::string& _path = "")
    : BoxCollider()
    , m_data(_data)
  {
    m_model_name = _modelName;
    m_dots = _dots;
    m_center = _center;
    m_radius = _radius;
    m_path = _path;
  }

  virtual void CalculateExtremDots(const eObject* _object) override;

  virtual std::vector<glm::mat3>	GetBoundingTriangles(const ITransform& trans)const override;
  virtual std::vector<glm::vec3>	GetExtrems(const ITransform& trans) const override;
  virtual glm::vec3               GetCenter() override;
  virtual float										GetRadius() override;

  const std::vector<AnimationData>& GetData() const { return m_data; }
  const IRigger*                    GetRigger() const { return m_rigger;}

protected:
  std::vector<AnimationData> m_data;
  IRigger*                   m_rigger = nullptr;
};

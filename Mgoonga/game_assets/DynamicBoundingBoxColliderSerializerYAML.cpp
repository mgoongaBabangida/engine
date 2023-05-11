#include "DynamicBoundingBoxColliderSerializerYAML.h"
#include "YamlTyps.h"

#include <math/BoxColliderDynamic.h>

#include <fstream>

//-------------------------------------------------------------------------------------------------
static void SerializeAnimationData(YAML::Emitter& _out, const BoxColliderDynamic::AnimationData& _animData)
{
  _out << YAML::BeginMap;
  _out << YAML::Key << "AnimationName" << YAML::Value << _animData.name;
  _out << YAML::Key << "DataNodes" << YAML::Value << YAML::BeginSeq;
  for (unsigned int i = 0 ; i < _animData.extremDots.size(); ++i)
  {
    _out << YAML::BeginMap;

    _out << YAML::Key << "Center" << YAML::Value << _animData.centers[i];
    _out << YAML::Key << "Radius" << YAML::Value << _animData.radiuses[i];
    _out << YAML::Key << "MaxX" << YAML::Value << _animData.extremDots[i].MaxX;
    _out << YAML::Key << "MaxY" << YAML::Value << _animData.extremDots[i].MaxY;
    _out << YAML::Key << "MaxZ" << YAML::Value << _animData.extremDots[i].MaxZ;
    _out << YAML::Key << "MinX" << YAML::Value << _animData.extremDots[i].MinX;
    _out << YAML::Key << "MinY" << YAML::Value << _animData.extremDots[i].MinY;
    _out << YAML::Key << "MinZ" << YAML::Value << _animData.extremDots[i].MinZ;

    _out << YAML::EndMap;
  }
  _out << YAML::EndSeq;
  _out << YAML::EndMap;
}

//-------------------------------------------------------------------------------------------------
void DynamicBoundingBoxColliderSerializerYAML::Serialize(BoxColliderDynamic* _box, const std::string& _filepath)
{
  YAML::Emitter out;
  out << YAML::BeginMap; //BoxColliderDynamic
  out << YAML::Key << "BoxColliderDynamic" << YAML::Value << "Unnamed";
  out << YAML::Key << "ModelName" << YAML::Value << _box->GetModelName();
  out << YAML::Key << "Center" << YAML::Value << _box->BoxCollider::GetCenter();
  out << YAML::Key << "Radius" << YAML::Value << _box->BoxCollider::GetRadius();
  out << YAML::Key << "MaxX" << YAML::Value << _box->BoxCollider::GetExtremDotsLocalSpace().MaxX;
  out << YAML::Key << "MaxY" << YAML::Value << _box->BoxCollider::GetExtremDotsLocalSpace().MaxY;
  out << YAML::Key << "MaxZ" << YAML::Value << _box->BoxCollider::GetExtremDotsLocalSpace().MaxZ;
  out << YAML::Key << "MinX" << YAML::Value << _box->BoxCollider::GetExtremDotsLocalSpace().MinX;
  out << YAML::Key << "MinY" << YAML::Value << _box->BoxCollider::GetExtremDotsLocalSpace().MinY;
  out << YAML::Key << "MinZ" << YAML::Value << _box->BoxCollider::GetExtremDotsLocalSpace().MinZ;
  
  out << YAML::Key << "AnimationData" << YAML::Value << YAML::BeginSeq;
  for (const auto& animData : _box->GetData())
  {
    SerializeAnimationData(out, animData);
  }
  out << YAML::EndSeq;

  out << YAML::EndMap; //BoxColliderDynamic
  std::ofstream fout(_filepath);
  fout << out.c_str();
}

//-------------------------------------------------------------------------------------------------
BoxColliderDynamic* DynamicBoundingBoxColliderSerializerYAML::Deserialize(const std::string& _filepath)
{
  std::ifstream stream(_filepath);
  std::stringstream strstream;
  strstream << stream.rdbuf();

  YAML::Node data = YAML::Load(strstream.str());
  if (!data["BoxColliderDynamic"])
    return nullptr;

  std::string model_name = data["ModelName"].as<std::string>();
  glm::vec3 center = data["Center"].as<glm::vec3>();
  float radius = data["Radius"].as<float>();
  extremDots dots;
  dots.MaxX = data["MaxX"].as<float>();
  dots.MaxY = data["MaxY"].as<float>();
  dots.MaxZ = data["MaxZ"].as<float>();
  dots.MinX = data["MinX"].as<float>();
  dots.MinY = data["MinY"].as<float>();
  dots.MinZ = data["MinZ"].as<float>();

  std::vector<BoxColliderDynamic::AnimationData> animDataVector;
  auto animationDataComponent = data["AnimationData"];
  if (animationDataComponent)
  {
    for (const auto& animData : animationDataComponent)
    {
      std::string animation_name = animData["AnimationName"].as<std::string>();
      animDataVector.push_back({ animation_name });
      auto dataNodes = animData["DataNodes"];
      for (auto& node : dataNodes)
      {
        glm::vec3 center = node["Center"].as<glm::vec3>();
        float radius = node["Radius"].as<float>();
        extremDots dots;
        dots.MaxX = node["MaxX"].as<float>();
        dots.MaxY = node["MaxY"].as<float>();
        dots.MaxZ = node["MaxZ"].as<float>();
        dots.MinX = node["MinX"].as<float>();
        dots.MinY = node["MinY"].as<float>();
        dots.MinZ = node["MinZ"].as<float>();
        animDataVector.back().extremDots.push_back(dots);
        animDataVector.back().centers.push_back(center);
        animDataVector.back().radiuses.push_back(radius);
      }
    }
  }
  return new BoxColliderDynamic(model_name, dots, center, radius, animDataVector, _filepath);
}

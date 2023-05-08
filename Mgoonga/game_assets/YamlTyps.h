#pragma once

#include <yaml-cpp/include/yaml-cpp/yaml.h>

#include <glm\glm\vec3.hpp>
#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>

YAML::Emitter& operator <<(YAML::Emitter& _out, glm::vec3 _vec)
{
  _out << YAML::Flow;
  _out << YAML::BeginSeq << _vec.x << _vec.y << _vec.z << YAML::EndSeq;
  return _out;
}

YAML::Emitter& operator <<(YAML::Emitter& _out, glm::quat _quat)
{
  _out << YAML::Flow;
  _out << YAML::BeginSeq << _quat.x << _quat.y << _quat.z << _quat.w << YAML::EndSeq;
  return _out;
}

namespace YAML
{
  template<>
  struct convert<glm::vec3>
  {
    static Node encode(const glm::vec3& rhs)
    {
      Node node;
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      node.push_back(rhs.z);
      return node;
    }

    static bool decode(const Node& node, glm::vec3& rhs)
    {
      if (!node.IsSequence() || node.size() != 3)
        return false;

      rhs.x = node[0].as<float>();
      rhs.y = node[1].as<float>();
      rhs.z = node[2].as<float>();
      return true;
    }
  };

  template<>
  struct convert<glm::quat>
  {
    static Node encode(const glm::quat& rhs)
    {
      Node node;
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      node.push_back(rhs.z);
      node.push_back(rhs.w);
      return node;
    }

    static bool decode(const Node& node, glm::quat& rhs)
    {
      if (!node.IsSequence() || node.size() != 4)
        return false;

      rhs.x = node[0].as<float>();
      rhs.y = node[1].as<float>();
      rhs.z = node[2].as<float>();
      rhs.w = node[3].as<float>();
      return true;
    }
  };
}

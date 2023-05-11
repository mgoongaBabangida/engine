#pragma once

#include <yaml-cpp/include/yaml-cpp/yaml.h>

#include <glm\glm\vec3.hpp>
#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>
#include <glm\glm\mat4x4.hpp>

YAML::Emitter& operator <<(YAML::Emitter& _out, glm::mat4 _mat);

YAML::Emitter& operator <<(YAML::Emitter& _out, glm::ivec4 _vec);

YAML::Emitter& operator <<(YAML::Emitter& _out, glm::vec4 _vec);

YAML::Emitter& operator <<(YAML::Emitter& _out, glm::vec3 _vec);

YAML::Emitter& operator <<(YAML::Emitter& _out, glm::vec2 _vec);

YAML::Emitter& operator <<(YAML::Emitter& _out, glm::quat _quat);

YAML::Emitter& operator <<(YAML::Emitter& _out, std::vector<unsigned int> _indeces);

namespace YAML
{
  template<>
  struct convert<glm::mat4>
  {
    static Node encode(const glm::mat4& rhs)
    {
      Node node;
      node.push_back(rhs[0][0]);
      node.push_back(rhs[0][1]);
      node.push_back(rhs[0][2]);
      node.push_back(rhs[0][3]);

      node.push_back(rhs[1][0]);
      node.push_back(rhs[1][1]);
      node.push_back(rhs[1][2]);
      node.push_back(rhs[1][3]);

      node.push_back(rhs[2][0]);
      node.push_back(rhs[2][1]);
      node.push_back(rhs[2][2]);
      node.push_back(rhs[2][3]);

      node.push_back(rhs[3][0]);
      node.push_back(rhs[3][1]);
      node.push_back(rhs[3][2]);
      node.push_back(rhs[3][3]);
      return node;
    }

    static bool decode(const Node& node, glm::mat4& rhs)
    {
      if (!node.IsSequence())
        return false;

      rhs[0][0] = node[0].as<float>();
      rhs[0][1] = node[1].as<float>();
      rhs[0][2] = node[2].as<float>();
      rhs[0][3] = node[3].as<float>();

      rhs[1][0] = node[4].as<float>();
      rhs[1][1] = node[5].as<float>();
      rhs[1][2] = node[6].as<float>();
      rhs[1][3] = node[7].as<float>();

      rhs[2][0] = node[8].as<float>();
      rhs[2][1] = node[9].as<float>();
      rhs[2][2] = node[10].as<float>();
      rhs[2][3] = node[11].as<float>();

      rhs[3][0] = node[12].as<float>();
      rhs[3][1] = node[13].as<float>();
      rhs[3][2] = node[14].as<float>();
      rhs[3][3] = node[15].as<float>();

      return true;
    }
  };

  template<>
  struct convert<std::vector<unsigned int>>
  {
    static Node encode(const std::vector<unsigned int>& rhs)
    {
      Node node;
      for(auto& index : rhs)
        node.push_back(index);
      return node;
    }

    static bool decode(const Node& node, std::vector<unsigned int>& rhs)
    {
      if (!node.IsSequence())
        return false;

      for (auto& index : node)
        rhs.push_back(index.as<unsigned int>());
      return true;
    }
  };

  template<>
  struct convert<glm::ivec4>
  {
    static Node encode(const glm::ivec4& rhs)
    {
      Node node;
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      node.push_back(rhs.z);
      node.push_back(rhs.w);
      return node;
    }

    static bool decode(const Node& node, glm::ivec4& rhs)
    {
      if (!node.IsSequence() || node.size() != 4)
        return false;

      rhs.x = node[0].as<int>();
      rhs.y = node[1].as<int>();
      rhs.z = node[2].as<int>();
      rhs.w = node[3].as<int>();
      return true;
    }
  };

  template<>
  struct convert<glm::vec4>
  {
    static Node encode(const glm::vec4& rhs)
    {
      Node node;
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      node.push_back(rhs.z);
      node.push_back(rhs.w);
      return node;
    }

    static bool decode(const Node& node, glm::vec4& rhs)
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
  struct convert<glm::vec2>
  {
    static Node encode(const glm::vec2& rhs)
    {
      Node node;
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      return node;
    }

    static bool decode(const Node& node, glm::vec2& rhs)
    {
      if (!node.IsSequence() || node.size() != 2)
        return false;

      rhs.x = node[0].as<float>();
      rhs.y = node[1].as<float>();
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

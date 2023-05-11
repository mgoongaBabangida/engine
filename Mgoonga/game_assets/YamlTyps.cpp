#include "YamlTyps.h"

YAML::Emitter& operator<<(YAML::Emitter& _out, glm::mat4 _mat)
{
  _out << YAML::Flow;
  _out << YAML::BeginSeq << _mat[0][0] << _mat[0][1] << _mat[0][2] << _mat[0][3];
                    _out << _mat[1][0] << _mat[1][1] << _mat[1][2] << _mat[1][3];
                    _out << _mat[2][0] << _mat[2][1] << _mat[2][2] << _mat[2][3];
                    _out << _mat[3][0] << _mat[3][1] << _mat[3][2] << _mat[3][3] << YAML::EndSeq;
  return _out;
}

YAML::Emitter& operator<<(YAML::Emitter& _out, glm::ivec4 _vec)
{
  _out << YAML::Flow;
  _out << YAML::BeginSeq << _vec.x << _vec.y << _vec.z << _vec.w << YAML::EndSeq;
  return _out;
}

YAML::Emitter& operator<<(YAML::Emitter& _out, glm::vec4 _vec)
{
  _out << YAML::Flow;
  _out << YAML::BeginSeq << _vec.x << _vec.y << _vec.z << _vec.w << YAML::EndSeq;
  return _out;
}

YAML::Emitter& operator<<(YAML::Emitter& _out, glm::vec3 _vec)
{
  _out << YAML::Flow;
  _out << YAML::BeginSeq << _vec.x << _vec.y << _vec.z << YAML::EndSeq;
  return _out;
}

YAML::Emitter& operator<<(YAML::Emitter& _out, glm::vec2 _vec)
{
  _out << YAML::Flow;
  _out << YAML::BeginSeq << _vec.x << _vec.y << YAML::EndSeq;
  return _out;
}

YAML::Emitter& operator<<(YAML::Emitter& _out, glm::quat _quat)
{
  _out << YAML::Flow;
  _out << YAML::BeginSeq << _quat.x << _quat.y << _quat.z << _quat.w << YAML::EndSeq;
  return _out;
}

YAML::Emitter& operator<<(YAML::Emitter& _out, std::vector<unsigned int> _indeces)
{
  _out << YAML::Flow;
  _out << YAML::BeginSeq;
  for (auto& index : _indeces)
    _out << index;
  _out << YAML::EndSeq;
  return _out;
}

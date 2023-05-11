#include "AnimationManagerYAML.h"
#include "YamlTyps.h"

#include <math/Rigger.h>

#include <algorithm>
#include <fstream>

static void SerializeAnimation(YAML::Emitter& _out, const SceletalAnimation& _animation)
{
  _out << YAML::BeginMap; //Animation
  _out << YAML::Key << "Name" << YAML::Value << _animation.Name();
  _out << YAML::Key << "Duration" << YAML::Value << _animation.GetDuration();

  _out << YAML::Key << "Frames" << YAML::BeginSeq;
  for (unsigned int i = 0; i < _animation.GetNumFrames(); ++i)
  {
    Frame frame = _animation.GetFrameByNumber(i);
    _out << YAML::BeginMap; // Frames
    _out << YAML::Key << "TimeStamp" << YAML::Value << frame.timeStamp;
    _out << YAML::Key << "Positions" << YAML::BeginSeq;
    for (auto& p : frame.pose)
    {
      _out << YAML::BeginMap; //Pos
      _out << YAML::Key << "BoneName" << YAML::Value << p.first;
      _out << YAML::Key << "Translation" << YAML::Value << p.second.getTranslation();
      _out << YAML::Key << "Rotation" << YAML::Value << p.second.getRotation();
      _out << YAML::Key << "Scale" << YAML::Value << p.second.getScaleAsVector();
      _out << YAML::EndMap; //Pos
    }
    _out << YAML::EndSeq;
    _out << YAML::EndMap;// Frames
  }
  _out << YAML::EndSeq;
  _out << YAML::EndMap; //Animation
}

//-----------------------------------------------------------
void AnimationManagerYAML::AddAnimation(const SceletalAnimation& _animation)
{
  m_animation.push_back(_animation);
}

//-----------------------------------------------------------
std::optional<SceletalAnimation> AnimationManagerYAML::GetAnimation(const std::string& _name) const
{
  if (auto it = std::find_if(m_animation.begin(), m_animation.end(), [_name](const SceletalAnimation& _anim) { return _anim.Name() == _name; });
    it != m_animation.end())
    return *it;
  else
    return std::nullopt;
}

//-----------------------------------------------------------
void AnimationManagerYAML::Serialize(const std::string& _filepath)
{
  YAML::Emitter out;
  out << YAML::BeginMap;
  out << YAML::Key << "AnimationData" << YAML::Value << "Unnamed";
  out << YAML::Key << "Animations" << YAML::Value << YAML::BeginSeq;
  for (auto& animation : m_animation)
  {
    SerializeAnimation(out, animation);
  }
  out << YAML::EndSeq;
  out << YAML::EndMap;

  std::ofstream fout(_filepath);
  fout << out.c_str();
}

//-----------------------------------------------------------
bool AnimationManagerYAML::Deserialize(const std::string& _filepath)
{
  std::ifstream stream(_filepath);
  std::stringstream strstream;
  strstream << stream.rdbuf();

  YAML::Node data = YAML::Load(strstream.str());
  if (!data["AnimationData"])
    return false;

  auto serialized_animations = data["Animations"];
  if (serialized_animations)
  {
    for (auto serialized_animation : serialized_animations)
    {
      int64_t duration; std::vector<Frame> frames; std::string name;
      name = serialized_animation["Name"].as<std::string>();
      duration = serialized_animation["Duration"].as<int64_t>();
      auto framesComponent = serialized_animation["Frames"];
      if (framesComponent)
      {
        for (auto& frame : framesComponent)
        {
          int64_t timeStamp = frame["TimeStamp"].as<int64_t>();
          std::map<std::string, Transform> pose;
          for (auto& pos : frame["Positions"])
          {
            std::string bone = pos["BoneName"].as<std::string>();
            Transform transform;
            transform.setTranslation(pos["Translation"].as<glm::vec3>());
            transform.setRotation(pos["Rotation"].as<glm::quat>());
            transform.setScale(pos["Scale"].as<glm::vec3>());
            pose.insert({ bone, transform });
          }
          frames.push_back(Frame{ timeStamp, pose });
        }
      }
      m_animation.emplace_back(duration, frames, name);
    }
  }
  return true;
}

//------------------------------------------------------------------------
static void _SerializeBone(YAML::Emitter& _out, const IBone* _bone)
{
  _out << YAML::BeginMap; //Bone
  _out << YAML::Key << "BoneName" << YAML::Value << _bone->GetName();
  _out << YAML::Key << "BoneID" << YAML::Value << _bone->GetID();
  _out << YAML::Key << "BindTransform" << YAML::Value << _bone->GetLocalBindTransform();
  _out << YAML::Key << "MTransform" << YAML::Value << _bone->GetMTransform();
  _out << YAML::Key << "IsRealBone" << YAML::Value << _bone->IsRealBone();
  if (!_bone->GetChildren().empty())
  {
    _out << YAML::Key << "Children" << YAML::Value << YAML::BeginSeq;
    for (auto* child : _bone->GetChildren())
    {
      _out << YAML::BeginMap;
      _out << YAML::Key << "BoneName" << YAML::Value << child->GetName();
      _out << YAML::Key << "BoneID" << YAML::Value << child->GetID();
      _out << YAML::EndMap;
    }
    _out << YAML::EndSeq;
  }
  _out << YAML::EndMap; //Bone
}

//-----------------------------------------------------------
void AnimationManagerYAML::SerializeRigger(const Rigger* _riger, const std::string& _filepath)
{
  YAML::Emitter out;
  out << YAML::BeginMap; // Rigger
  out << YAML::Key << "Rigger" << YAML::Value << "232311132";
  out << YAML::Key << "RootBoneName" << YAML::Value << _riger->GetNameRootBone();
  out << YAML::Key << "GlobalModelTransform" << YAML::Value << _riger->GetGlobalModelTransform();

  out << YAML::Key << "Bones" << YAML::Value << YAML::BeginSeq;
  for (auto& bone : _riger->GetBones())
  {
    _SerializeBone(out, &bone);
  }
  out << YAML::EndSeq;

  out << YAML::Key << "Animations" << YAML::Value << YAML::BeginSeq;
  for (auto& animation : _riger->GetAnimations())
  {
    out << animation.Name();
  }
  out << YAML::EndSeq;

  out << YAML::EndMap; // Rigger
  std::ofstream fout(_filepath);
  fout << out.c_str();
}

//-----------------------------------------------------------
IRigger* AnimationManagerYAML::DeserializeRigger(const std::string& _filepath)
{
  std::ifstream stream(_filepath);
  std::stringstream strstream;
  strstream << stream.rdbuf();

  YAML::Node data = YAML::Load(strstream.str());
  if (!data["Rigger"])
    return nullptr;

  std::string rootBoneName = data["RootBoneName"].as<std::string>();
  glm::mat4 globaltransform = data["GlobalModelTransform"].as<glm::mat4>();

  std::vector<Bone> bones;
  bones.reserve(100); //!
  std::map<Bone*, std::vector<unsigned int>> bone_children_map;
  auto serialized_bones = data["Bones"];
  for (auto serialized_bone : serialized_bones)
  {
    auto name = serialized_bone["BoneName"].as<std::string>();
    auto id = serialized_bone["BoneID"].as<unsigned int>();
    glm::mat4 bindTransform = serialized_bone["BindTransform"].as<glm::mat4>();
    glm::mat4 mTransform = serialized_bone["MTransform"].as<glm::mat4>();
    bool isReal = serialized_bone["IsRealBone"].as<bool>();
    std::vector<unsigned int> children;
    auto childrenComponent = serialized_bone["Children"];
    if (childrenComponent)
    {
      for (auto& child : childrenComponent)
        children.push_back(child["BoneID"].as<unsigned int>());
    }
    bones.push_back(Bone(id, name, bindTransform, isReal));
    bone_children_map.insert({ &bones.back(), children });
    bones.back().setMTransform(mTransform);
  }
  for (auto& bone_node : bone_children_map)
  {
    for (auto& child_index : bone_node.second)
    {
      auto childIter = std::find_if(bones.begin(), bones.end(), [child_index](const Bone& bone)
        { return bone.GetID() == child_index; });
      bone_node.first->addChild(&(*childIter));
    }
  }
  std::vector<SceletalAnimation> animations;
  auto animationsComponent = data["Animations"];
  if (animationsComponent)
  {
    for (auto animation : animationsComponent)
    {
      std::optional<SceletalAnimation> anim = this->GetAnimation(animation.as<std::string>());
      if(anim.has_value())
        animations.push_back(*anim);
    }
  }
  IRigger* rigger = new Rigger(animations, bones, rootBoneName, globaltransform, _filepath);
  return rigger;
}

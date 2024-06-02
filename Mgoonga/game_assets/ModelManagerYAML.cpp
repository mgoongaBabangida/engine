#include "stdafx.h"

#include "ModelManagerYAML.h"
#include "Serializer.h"

#include <opengl_assets/Texture.h>
#include <opengl_assets/Model.h>
#include <opengl_assets/TextureManager.h>

#include "YamlTyps.h"

#include <fstream>

//@todo finish and test
//------------------------------------------------------------------------
IModel* ModelManagerYAML::Add(const std::string& _name, char* _path, bool _invert_y_uv)
{
  std::string path(_path);
  if (path.find("mgoongaObject3d") == std::string::npos)
    return eModelManager::Add(_name, _path, _invert_y_uv);

  return _LoadModel(_name, path);
}

//------------------------------------------------------------------------
void ModelManagerYAML::Add(const std::string& _name, Primitive _type, Material&& _material)
{
  eModelManager::Add(_name, _type, std::move(_material));
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

//------------------------------------------------------------------------
static void _SerializeMesh(YAML::Emitter& _out, const eMesh* _mesh)
{
  _out << YAML::BeginMap; //Mesh

  _out << YAML::Key << "MeshName" << YAML::Value << _mesh->Name();
  _out << YAML::Key << "MeshPath" << YAML::Value << _mesh->Name() + ".mgoongaMesh";

  Serializer::WriteMeshToFile(*_mesh, _mesh->Name() + ".mgoongaMesh");

  _out << YAML::EndMap; //Mesh
}

//------------------------------------------------------------------------
void ModelManagerYAML::Save(eModel* _model, const std::string& _filepath)
{
  YAML::Emitter out;
  out << YAML::BeginMap;
  out << YAML::Key << "Model3D" << YAML::Value << _model->GetName();
  out << YAML::Key << "Init Path" << YAML::Value << _model->GetPath();
  
  out << YAML::Key << "Bones" << YAML::Value << YAML::BeginSeq;
  for (auto* bone : _model->GetBones())
  {
    _SerializeBone(out, bone);
  }
  out << YAML::EndSeq;

  out << YAML::Key << "Meshes" << YAML::Value << YAML::BeginSeq;
  for (int i = 0; i < _model->GetMeshCount(); ++i)
  {
    _SerializeMesh(out, _model->GetMeshByIndex(i));
  }
  out << YAML::EndSeq;
  out << YAML::EndMap;

  std::ofstream fout(_filepath);
  fout << out.c_str();
}

//------------------------------------------------------------------------
IModel* ModelManagerYAML::_LoadModel(const std::string& _name, const std::string& _filepath)
{
  std::ifstream stream(_filepath);
  std::stringstream strstream;
  strstream << stream.rdbuf();

  YAML::Node data = YAML::Load(strstream.str());
  if (!data["Model3D"])
    return false;

  std::string path = data["Init Path"].as<std::string>();

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

  eModel* model = new eModel(_name, {}, bones);
  auto serialized_meshes = data["Meshes"];
  for (auto serialized_mesh : serialized_meshes)
  {
    std::string name = serialized_mesh["MeshName"].as<std::string>();
    std::string path = serialized_mesh["MeshPath"].as<std::string>();

    Serializer::ReadMeshFromFile(*model, path);
  }

  models.insert(std::pair<std::string, std::shared_ptr<IModel> >(_name, model));
  return model;
}

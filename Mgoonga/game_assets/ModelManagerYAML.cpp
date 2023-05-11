#include "ModelManagerYAML.h"

#include <opengl_assets/Texture.h>
#include <opengl_assets/TextureManager.h>

#include "YamlTyps.h"

#include <fstream>

//------------------------------------------------------------------------
void ModelManagerYAML::Add(const std::string& _name, char* _path, bool _invert_y_uv)
{
  std::string path(_path);
  if (path.find("mgoongaObject3d") == std::string::npos)
    return eModelManager::Add(_name, _path, _invert_y_uv);

  _LoadModel(_name, path);
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
static void _SerializeMesh(YAML::Emitter& _out, const I3DMesh* _mesh)
{
  _out << YAML::BeginMap; //Mesh

  _out << YAML::Key << "MeshName" << YAML::Value << _mesh->Name();

  //Data information
  _out << YAML::BeginMap; //Data
  _out << YAML::Key << "Vertex Count" << YAML::Value << _mesh->GetVertexCount();
  _out << YAML::Key << "Vertex Path" << YAML::Value << _mesh->Name() + ".mgoongaVertex";
  _out << YAML::Key << "Index Count" << YAML::Value << _mesh->GetIndices().size();
  _out << YAML::Key << "Index Path" << YAML::Value << _mesh->Name() + ".mgoongaIndex";
  _out << YAML::EndMap; // Data

  //if (_mesh->GetVertexCount() != 0)
  //{
  //  _out << YAML::Key << "Vertices" << YAML::Value << YAML::BeginSeq;
  //  for (const auto& vertex : _mesh->GetVertexs())
  //  {
  //    _out << YAML::BeginMap; //Vertex
  //    _out << YAML::Key << "Position" << YAML::Value << vertex.Position; //0
  //    _out << YAML::Key << "Normal" << YAML::Value << vertex.Normal; //2
  //    _out << YAML::Key << "TexCoords" << YAML::Value << vertex.TexCoords; //3
  //    _out << YAML::Key << "tangent" << YAML::Value << vertex.tangent; //4
  //    _out << YAML::Key << "bitangent" << YAML::Value << vertex.bitangent; //5
  //    _out << YAML::Key << "boneIDs" << YAML::Value << vertex.boneIDs; //6
  //    _out << YAML::Key << "weights" << YAML::Value << vertex.weights; //7
  //    _out << YAML::EndMap; //Vertex
  //  }
  //  _out << YAML::EndSeq;
  //}

  /*_out << YAML::Key << "Indeces" << YAML::Value << YAML::BeginSeq;
  _out << _mesh->GetIndices();
  _out << YAML::EndSeq;*/

  if (_mesh->HasMaterial())
  {
    Material m = *_mesh->GetMaterial();

    _out << YAML::Key << "Material";
    _out << YAML::BeginMap; //Material

    _out << YAML::Key << "albedo"     << YAML::Value << m.albedo;
    _out << YAML::Key << "metallic"   << YAML::Value << m.metallic;
    _out << YAML::Key << "roughness"  << YAML::Value << m.roughness;
    _out << YAML::Key << "ao"         << YAML::Value << m.ao;

    _out << YAML::Key << "use_albedo" << YAML::Value << m.use_albedo;
    _out << YAML::Key << "use_metalic" << YAML::Value << m.use_metalic;
    _out << YAML::Key << "use_normal" << YAML::Value << m.use_normal;
    _out << YAML::Key << "use_roughness" << YAML::Value << m.use_roughness;

    _out << YAML::Key << "use_phong_shading" << YAML::Value << m.use_phong_shading;

    if (m.albedo_texture_id != Texture::GetDefaultTextureId())
    {
      for (auto& t : _mesh->GetTextures())
        if (t.m_type == "texture_diffuse")
          _out << YAML::Key << "albedo_texture_path" << YAML::Value << t.m_path;
    }
    if (m.metalic_texture_id != Texture::GetDefaultTextureId())
    {
      for (auto& t : _mesh->GetTextures())
        if (t.m_type == "texture_specular")
          _out << YAML::Key << "metalic_texture_path" << YAML::Value << t.m_path;
    }
    if (m.normal_texture_id != Texture::GetDefaultTextureId())
    {
      for (auto& t : _mesh->GetTextures())
        if (t.m_type == "texture_normal")
          _out << YAML::Key << "normal_texture_path" << YAML::Value << t.m_path;
    }
    if (m.roughness_texture_id != Texture::GetDefaultTextureId())
    {
      for (auto& t : _mesh->GetTextures())
        if (t.m_type == "texture_roughness")
          _out << YAML::Key << "roughness_texture_path" << YAML::Value << t.m_path;
    }
    if (m.emissive_texture_id != Texture::GetDefaultTextureId())
    {
      for (auto& t : _mesh->GetTextures())
        if (t.m_type == "texture_emission")
          _out << YAML::Key << "emissive_texture_path" << YAML::Value << t.m_path;
    }

    _out << YAML::EndMap; //Material
  }

  _out << YAML::EndMap; //Mesh
}

//------------------------------------------------------------------------
void ModelManagerYAML::Save(IModel* _model, const std::string& _filepath)
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
  for (auto* mesh : _model->Get3DMeshes())
  {
    _SerializeMesh(out, mesh);
  }
  out << YAML::EndSeq;
  out << YAML::EndMap;

  std::ofstream fout(_filepath);
  fout << out.c_str();
}

//------------------------------------------------------------------------
bool ModelManagerYAML::_LoadModel(const std::string& _name, const std::string& _filepath)
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

  auto serialized_meshes = data["Meshes"];
  for (auto serialized_mesh : serialized_meshes)
  {
    std::string name = serialized_mesh["MeshName"].as<std::string>();
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indeces;
    std::vector<Texture> textures;
    Material m;

    auto verticesComponent = serialized_mesh["Vertices"];
    if (verticesComponent)
    {
      for (auto vertex : verticesComponent)
      {
        Vertex v;
        v.Position = vertex["Position"].as<glm::vec3>(); // 0
        v.Normal = vertex["Normal"].as<glm::vec3>(); // 2
        v.TexCoords = vertex["TexCoords"].as<glm::vec2>(); // 3
        v.tangent = vertex["tangent"].as<glm::vec3>(); // 4
        v.bitangent = vertex["bitangent"].as<glm::vec3>(); // 5
        v.boneIDs = vertex["boneIDs"].as<glm::ivec4>(); // 6
        v.weights = vertex["weights"].as<glm::vec4>(); // 7
        vertices.push_back(v);
      }
    }

    auto indecesComponent = serialized_mesh["Indeces"];
    if (indecesComponent)
    {
      for (auto index : indecesComponent)
      {
        indeces.push_back(index.as<unsigned int>());
      }
    }

    auto materialComponent = serialized_mesh["Material"];
    if (materialComponent)
    {
      m.albedo = materialComponent["albedo"].as<glm::vec3>();
      m.metallic = materialComponent["metallic"].as<float>();
      m.roughness = materialComponent["roughness"].as<float>();
      m.ao = materialComponent["ao"].as<float>();
      m.use_albedo = materialComponent["use_albedo"].as<bool>();
      m.use_metalic = materialComponent["use_metalic"].as<bool>();
      m.use_normal = materialComponent["use_normal"].as<bool>();
      m.use_roughness = materialComponent["use_roughness"].as<bool>();
      m.use_phong_shading = materialComponent["use_phong_shading"].as<bool>();

      std::string albedo_path, metallic_path, normal_path, roughness_path, emissive_path;
      auto albedo = materialComponent["albedo_texture_path"];
      if (albedo)
        albedo_path = albedo.as<std::string>();
      auto metallic = materialComponent["metalic_texture_path"];
      if(metallic)
        metallic_path = metallic.as<std::string>();
      auto normal = materialComponent["normal_texture_path"];
      if(normal)
        normal_path = normal.as<std::string>();
      auto roughnes = materialComponent["roughness_texture_path"];
      if(roughnes)
        roughness_path = roughnes.as<std::string>();
      auto emissive = materialComponent["emissive_texture_path"];
      if(emissive)
        emissive_path = emissive.as<std::string>();
    }
  }

  /*IModel* model = nullptr;
  models.insert(std::pair<std::string, std::shared_ptr<IModel> >(_name, model));*/
  return true;
}

#include "SceneSerializerYAML.h"

#include "YamlTyps.h"
#include "ObjectFactory.h"
#include "AnimationManagerYAML.h"
#include "ModelManagerYAML.h"

#include <base/Object.h>

#include <fstream>

//---------------------------------------------------------------------------------------------
static void SerializeObject(YAML::Emitter& _out, std::shared_ptr<eObject> _object)
{
  _out << YAML::BeginMap; //Object
  _out << YAML::Key << "Object";
  _out << YAML::Value << "Guid-23231-dsd0323";
  _out << YAML::Key << "RenderingType" << YAML::Value << (int64_t)_object->GetRenderType();

  if (!_object->Name().empty())
  {
    _out << YAML::Key << "NameComponent";
    _out << YAML::BeginMap;
    _out << YAML::Key << "Name" << YAML::Value << _object->Name();
    _out << YAML::EndMap;
  }

  if (IModel* model = _object->GetModel(); model != nullptr)
  {
    _out << YAML::Key << "ModelComponent";
    _out << YAML::BeginMap;
    _out << YAML::Key << "ModelName" << YAML::Value << _object->GetModel()->GetName();
    _out << YAML::Key << "Path" << YAML::Value << _object->GetModel()->GetPath();
    _out << YAML::EndMap;
  }

  if (ITransform* transform = _object->GetTransform(); transform != nullptr)
  {
    _out << YAML::Key << "TransformComponent";
    _out << YAML::BeginMap;
    _out << YAML::Key << "Translation" << YAML::Value << transform->getTranslation();
    _out << YAML::Key << "Rotation" << YAML::Value << transform->getRotation();
    _out << YAML::Key << "Scale" << YAML::Value << transform->getScaleAsVector();
    _out << YAML::Key << "Forward" << YAML::Value << transform->getForward();
    _out << YAML::Key << "Up" << YAML::Value << transform->getUp();
    _out << YAML::EndMap;
  }

  if (IRigger* rigger = _object->GetRigger(); rigger != nullptr)
  {
    _out << YAML::Key << "RiggerComponent";
    
    auto path = rigger->GetPath().empty() ? "Default" : rigger->GetPath();
    _out << YAML::BeginMap;
    _out << YAML::Key << "Path" << YAML::Value << path;
    _out << YAML::EndMap;
  }

  if (ICollider* collider = _object->GetCollider(); collider != nullptr)
  {
    if (!collider->GetPath().empty())
    {
      _out << YAML::Key << "ColliderComponent";
      _out << YAML::BeginMap;
      _out << YAML::Key << "Path" << YAML::Value << collider->GetPath();
      _out << YAML::EndMap;
    }
  }

  _out << YAML::EndMap; //Object
}

//-------------------------------------------------------------
SceneSerializerYAML::SceneSerializerYAML(std::vector<std::shared_ptr<eObject>> _objects,
                                        ModelManagerYAML& _model_manager,
                                        AnimationManagerYAML& _animationManager)
  : m_objects(_objects)
  , m_model_manager(_model_manager)
  , m_animation_manager(_animationManager)
{
}

//-------------------------------------------------------------
void SceneSerializerYAML::Serialize(const std::string& _filepath)
{
  YAML::Emitter out;
  out << YAML::BeginMap;
  out << YAML::Key << "Scene" << YAML::Value << "Unnamed";
  out << YAML::Key << "Objects" << YAML::Value << YAML::BeginSeq;
  for (auto& object : m_objects)
  {
    SerializeObject(out, object);
  }
  out << YAML::EndSeq;
  out << YAML::EndMap;

  std::ofstream fout(_filepath);
  fout << out.c_str();
}

//-------------------------------------------------------------
std::vector<std::shared_ptr<eObject>> SceneSerializerYAML::Deserialize(const std::string& _filepath)
{
  std::ifstream stream(_filepath);
  std::stringstream strstream;
  strstream << stream.rdbuf();

  YAML::Node data = YAML::Load(strstream.str());
  if(!data["Scene"])
    return {};

  std::string sceneName = data["Scene"].as<std::string>();
  auto serialized_objects = data["Objects"];
  if (serialized_objects)
  {
    ObjectFactoryBase factory(&m_animation_manager.get());
    for (auto serialized_object : serialized_objects)
    {
      std::string id = serialized_object["Object"].as<std::string>(); //guid
      eObject::RenderType rendering_type = (eObject::RenderType)serialized_object["RenderingType"].as<int64_t>();
      std::string object_name;
      auto nameComponent = serialized_object["NameComponent"];
      if (nameComponent)
        object_name = nameComponent["Name"].as<std::string>();

      std::string model_name;
      auto modelComponent = serialized_object["ModelComponent"];
      if (modelComponent)
      {
        model_name = modelComponent["ModelName"].as<std::string>();
        std::string path = modelComponent["Path"].as<std::string>();
      }

      std::string rigger_path;
      auto riggerComponent = serialized_object["RiggerComponent"];
      if (riggerComponent)
      {
        auto pathNode = riggerComponent["Path"];
        if(pathNode)
          rigger_path = pathNode.as<std::string>();
      }

      std::string collider_path;
      auto colliderComponent = serialized_object["ColliderComponent"];
      if (colliderComponent)
      {
        auto pathNode = colliderComponent["Path"];
        if (pathNode)
          collider_path = pathNode.as<std::string>();
      }
      std::shared_ptr<eObject> object;
      if (riggerComponent)
      {
        object = factory.CreateObject(m_model_manager.get().Find(model_name),
                                      rendering_type,
                                      object_name,
                                      rigger_path,
                                      collider_path,
                                      !collider_path.empty());
      }
      else
      {
        object = factory.CreateObject(m_model_manager.get().Find(model_name), rendering_type, object_name);
      }

      auto transformComponent = serialized_object["TransformComponent"];
      if (transformComponent)
      {
        object->GetTransform()->setTranslation(transformComponent["Translation"].as<glm::vec3>());
        object->GetTransform()->setRotation(transformComponent["Rotation"].as<glm::quat>());
        object->GetTransform()->setScale(transformComponent["Scale"].as<glm::vec3>());
        object->GetTransform()->setForward(transformComponent["Forward"].as<glm::vec3>());
        object->GetTransform()->setUp(transformComponent["Up"].as<glm::vec3>());
      }
      m_objects.push_back(object);
    } // serialized object
  }
  return m_objects;
}

#pragma once
#include <base/interfaces.h>
#include <opengl_assets/ModelManager.h>

class ModelManagerYAML : public eModelManager
{
public:
  virtual void	Add(const std::string& name, char* path, bool invert_y_uv = false);
  
  //@todo temp
  virtual void Add(const std::string& name, std::vector<const Texture*> _t) {
    return eModelManager::Add(name, _t);
  }
  virtual void                  Add(const std::string& name){
    return eModelManager::Add(name);
  }

  virtual void Save(IModel* _model, const std::string& _path);
protected:
  bool _LoadModel(const std::string& _name, const std::string& _path);
};

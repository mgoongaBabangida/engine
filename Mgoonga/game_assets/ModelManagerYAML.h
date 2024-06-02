#pragma once
#include <base/interfaces.h>
#include <opengl_assets/ModelManager.h>

class eModel;

//------------------------------------------------------------------------------------------
class ModelManagerYAML : public eModelManager
{
public:
  virtual IModel* Add(const std::string& name, char* path, bool invert_y_uv = false);
  virtual void    Add(const std::string& _name, Primitive _type, Material&& _material);
  virtual void    Save(eModel* _model, const std::string& _path);

protected:
  IModel* _LoadModel(const std::string& _name, const std::string& _path);
};

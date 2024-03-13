#pragma once

#include "stdafx.h"
#include "opengl_assets.h"
#include "MyModel.h"
#include "AssimpModel.h"
#include "TerrainModel.h"

#include <set>

enum class Primitive
{
  SPHERE = 0
};

//----------------------------------------------------------------------
class DLL_OPENGL_ASSETS eModelManager
{
  public:
  	eModelManager();
    ~eModelManager();

  	void							            InitializePrimitives();
  	std::shared_ptr<IModel>			  Find(const std::string& name) const;

  	virtual void							    Add(const std::string& name, char* path, bool invert_y_uv = false);
    virtual void                  Add(const std::string& _name, Primitive _type, Material&& _material);
    
    void							            AddPrimitive(const std::string&, std::shared_ptr<MyModel>);

  	std::unique_ptr<MyModel>		  ClonePrimitive(const std::string& name);
  	std::unique_ptr<TerrainModel>	CloneTerrain(const std::string& name);
  	std::shared_ptr<MyMesh>			  FindMesh(const std::string&);
  
  protected:
  	std::map<std::string, std::shared_ptr<MyMesh> >		myMeshes;
  	std::map<std::string, std::shared_ptr<IModel> >   models;
  	std::map<std::string, std::shared_ptr<MyModel> >	primitves;
  	std::unique_ptr<TerrainModel>						          terrain;
};

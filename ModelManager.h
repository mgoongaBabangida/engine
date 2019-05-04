#pragma once
#include "MyModel.h"
#include "AssimpModel.h"
#include "TerrainModel.h"

#include <set>
#include <memory>
#include <string>

class eModelManager
{
public:
	eModelManager();
	void							InitializePrimitives();
	std::shared_ptr<IModel>			Find(const std::string& name);
	void							Add(const std::string& name, GLchar* path);
	void							AddPrimitive(const std::string&, std::shared_ptr<MyModel>);
	std::unique_ptr<MyModel>		ClonePrimitive(const std::string& name);
	std::unique_ptr<TerrainModel>	CloneTerrain(const std::string& name);
	std::shared_ptr<MyMesh>			FindMesh(const std::string&);
private:
	std::map<std::string, std::shared_ptr<MyMesh> >		myMeshes;
	std::map<std::string, std::shared_ptr<IModel> >		models;
	std::map<std::string, std::shared_ptr<MyModel> >	primitves;
	std::unique_ptr<TerrainModel>						terrain;
};

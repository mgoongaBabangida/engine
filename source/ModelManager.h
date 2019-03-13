#pragma once
#include "MyModel.h" //temp
#include "AssimpModel.h" //temp
#include "TerrainModel.h" //temp

class ModelManager
{
private:
	std::map<std::string, std::shared_ptr<MyMesh> > myMeshes;
	std::map < std::string, std::shared_ptr<IModel> > models;
	std::map < std::string, std::shared_ptr<MyModel> > primitves;
	std::unique_ptr<TerrainModel> terrain;
public:
	ModelManager();
	void initializePrimitives();
	std::shared_ptr<IModel> find(std::string name);
	void add(std::string name, GLchar* path);
	void addPrimitive(std::string, std::shared_ptr<MyModel>);
	std::unique_ptr<MyModel> clonePrimitive(std::string name);
	std::unique_ptr<TerrainModel> cloneTerrain(std::string name);
	std::shared_ptr<MyMesh> findMesh(std::string);
};

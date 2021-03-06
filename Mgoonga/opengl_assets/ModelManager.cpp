#include "stdafx.h"
#include "ModelManager.h"
#include "ShapeGenerator.h"
#include "ShapeData.h"
#include "MyMesh.h"
#include "AssimpModel.h"

void eModelManager::InitializePrimitives()
{
	ShapeData cube = ShapeGenerator::makeCube();
	ShapeData arrow = ShapeGenerator::makeArrow();
	ShapeData plane = ShapeGenerator::makePlane();
	ShapeData quad = ShapeGenerator::makeQuad();
	ShapeData sphere = ShapeGenerator::makeSphere(40);
	ShapeData square = ShapeGenerator::makeSquare(5.0f, 5.0f); //(Width() / Height(),1.0f )

	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh>>("cube", new MyMesh(cube)));
	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh> >("plane", new MyMesh(plane)));
	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh> >("arrow", new MyMesh(arrow)));
	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh> >("quad", new MyMesh(quad)));
	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh> >("sphere", new MyMesh(sphere)));
	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh> >("square", new MyMesh(square)));

	cube.cleanup();
	arrow.cleanup();
	plane.cleanup();
	quad.cleanup();
	sphere.cleanup();
	square.cleanup();
}

eModelManager::eModelManager(): terrain(new TerrainModel)
{}

std::shared_ptr<IModel> eModelManager::Find(const std::string& name)
{
	return  models.find(name)->second;
}

void eModelManager::Add(const std::string& name, GLchar* path, bool invert_y_uv)
{
	models.insert(std::pair<std::string, std::shared_ptr<IModel> >(name, new Model(path, invert_y_uv)) );
}

void eModelManager::AddPrimitive(const std::string& name, std::shared_ptr<MyModel> model)
{
	primitves.insert(std::pair<std::string, std::shared_ptr<MyModel> >(name, model));
	models.insert(std::pair<std::string, std::shared_ptr<IModel> >(name, new MyModel(*(model.get()))));
}

std::unique_ptr<MyModel> eModelManager::ClonePrimitive(const std::string& name)
{
	//if (primitves.find(name) == primitves.end())
		//assert
	std::shared_ptr<MyModel> model = primitves.find(name)->second;
	return std::unique_ptr<MyModel>(new MyModel(*(model.get())));
}

std::unique_ptr<TerrainModel> eModelManager::CloneTerrain(const std::string& name)
{
	return std::unique_ptr<TerrainModel>(new TerrainModel(*(terrain.get())));
}

std::shared_ptr<MyMesh> eModelManager::FindMesh(const std::string& name)
{
	return  myMeshes.find(name)->second;
}

#include "ModelManager.h"
#include "ShapeGenerator.h"
#include "ShapeData.h"
#include "MyMesh.h"
#include "AssimpModel.h"

void ModelManager::initializePrimitives()
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

ModelManager::ModelManager():terrain(new TerrainModel)
{

}

std::shared_ptr<IModel> ModelManager::find(std::string name)
{
	return  models.find(name)->second;
}

void ModelManager::add(std::string name, GLchar* path)
{
	models.insert(std::pair<std::string, std::shared_ptr<IModel> >(name, new Model(path)) );
}

void ModelManager::addPrimitive(std::string name, std::shared_ptr<MyModel> model)
{
	primitves.insert(std::pair<std::string, std::shared_ptr<MyModel> >(name, model));
	models.insert(std::pair<std::string, std::shared_ptr<IModel> >(name, new MyModel(*(model.get()))));   
}

std::unique_ptr<MyModel> ModelManager::clonePrimitive(std::string name)
{
	if (primitves.find(name) == primitves.end())
		std::cout << "ENDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD" << std::endl;
	std::shared_ptr<MyModel> model = primitves.find(name)->second;
	return std::unique_ptr<MyModel>(new MyModel(*(model.get())));
}

std::unique_ptr<TerrainModel> ModelManager::cloneTerrain(std::string name)
{
	return std::unique_ptr<TerrainModel>(new TerrainModel(*(terrain.get())));
}

std::shared_ptr<MyMesh> ModelManager::findMesh(std::string name)
{
	return  myMeshes.find(name)->second;	
}

#include "stdafx.h"

#include "ModelManager.h"

#include "ShapeGenerator.h"
#include "ShapeData.h"
#include "MyMesh.h"
#include "Model.h"
#include "AssimpLoader.h"
#include "ShpereTexturedModel.h"

#include "TextureManager.h"

void eModelManager::InitializePrimitives()
{
	ShapeData cube = ShapeGenerator::makeCube();
	ShapeData arrow = ShapeGenerator::makeArrow();
	ShapeData plane = ShapeGenerator::makePlane();
	ShapeData quad = ShapeGenerator::makeQuad();
	ShapeData sphere = ShapeGenerator::makeSphere(40);
	ShapeData square = ShapeGenerator::makeSquare(5.0f, 5.0f); //(Width() / Height(), 1.0f )
	ShapeData ellipse = ShapeGenerator::makeEllipse(2.0f, 4.0f, 2.0f, 10);

	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh>>("cube", new MyMesh("cube", cube)));
	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh> >("plane", new MyMesh("plane", plane)));
	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh> >("arrow", new MyMesh("arrow", arrow)));
	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh> >("quad", new MyMesh("quad", quad)));
	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh> >("sphere", new MyMesh("sphere", sphere)));
	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh> >("square", new MyMesh("square", square)));
	myMeshes.insert(std::pair<std::string, std::shared_ptr<MyMesh> >("ellipse", new MyMesh("ellipse", ellipse)));

	cube.cleanup();
	arrow.cleanup();
	plane.cleanup();
	quad.cleanup();
	sphere.cleanup();
	square.cleanup();
	ellipse.cleanup();
}

eModelManager::eModelManager()
	: terrain(new TerrainModel)
{}

eModelManager::~eModelManager()
{
}

std::shared_ptr<IModel> eModelManager::Find(const std::string& name) const
{
	return  models.find(name)->second;
}

IModel* eModelManager::Add(const std::string& name, char* path, bool invert_y_uv)
{
	AssimpLoader loader;
	IModel* model = loader.LoadModel(path, name, invert_y_uv);
	if (model)
	{
		bool fls = false;
		while (!container_flag.compare_exchange_weak(fls, true)) { fls = false; }
		models.insert(std::pair<std::string, std::shared_ptr<IModel> >(name, model));
		container_flag.store(false);
	}
	return model;
}

void eModelManager::Add(const std::string& _name, Primitive _type, Material&& _material)
{
	if (_type == Primitive::SPHERE)
	{
		SphereTexturedMesh* mesh = new SphereTexturedMesh();
		mesh->SetMaterial(std::move(_material)); //needs move ctr
		models.insert(std::pair<std::string, std::shared_ptr<IModel>>{ _name, new SphereTexturedModel(mesh) });
	}
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
	return std::make_unique<TerrainModel>(*(terrain));
}

std::shared_ptr<MyMesh> eModelManager::FindMesh(const std::string& name) const
{
	return  myMeshes.find(name)->second;
}

void eModelManager::ReloadTextures()
{
	for (auto& m : models)
	{
		m.second->SetUpMeshes();
		m.second->ReloadTextures();
	}
}

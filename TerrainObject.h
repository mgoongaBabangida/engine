#pragma once
#include "TerrainModel.h"
#include "Object.h"

class TerrainObject: public eObject
{
protected:
	ITerrainModel* terrain;
public:
	virtual void moveForward(std::vector<std::shared_ptr<eObject> > objects) override {};
	virtual void moveBack(std::vector<std::shared_ptr<eObject> > objects) override {};
	virtual void moveRight(std::vector<std::shared_ptr<eObject> > objects) override {};
	virtual void moveLeft(std::vector<std::shared_ptr<eObject> > objects) override {};

};

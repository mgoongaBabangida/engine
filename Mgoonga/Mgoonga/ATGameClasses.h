#pragma once

#include <base/interfaces.h>

#include "ShipScript.h"
#include <math/BoxCollider.h>
#include <math/RigidBdy.h>

//-------------------------------------------------------------------------------------------
class eShip : public eObject
{
public:
	eShip() = delete;
	eShip(IModel* _m, eShipScript* _script, bool _is_spanish, const std::string& _name = "empty")
		: eObject()
		, is_spanish(_is_spanish)
	{
		SetTransform(new Transform);
		SetCollider(new BoxCollider);
		SetRigidBody(new eRigidBody);
		SetModel(_m);
		SetName(_name);
		script.reset(_script);
		script->SetObject(this);
	}

	bool IsSpanish() { return is_spanish; }
	bool HasMoved() { return has_moved; }
	void SetHasMoved(bool m) { has_moved = m; }

	eShipScript* getShipScript() const { return static_cast<eShipScript*>(script.get()); }
protected:
	bool is_spanish;
	bool has_moved = false;
};

//-------------------------------------------------------------------------------------------
class eBase : public eObject
{
public:
	eBase() = delete;
	eBase(IModel* _m, eBaseScript* _script, const std::string& _name = "empty")
		: eObject()
	{
		SetTransform(new Transform);
		SetCollider(new BoxCollider);
		SetRigidBody(new eRigidBody);
		SetModel(_m);
		SetName(_name);
		script.reset(_script);
		script->SetObject(this);
	}
	eBaseScript* getBaseScript() const { return static_cast<eBaseScript*>(script.get()); }
};

//-------------------------------------------------------------------------------------------
class eTerrain : public eObject
{
public:
	eTerrain() = delete;
	eTerrain(ITerrainModel*_model, const std::string& _name = "empty")
		: eObject()
	{
		SetTransform(new Transform);
		SetCollider(new BoxCollider);
		SetRigidBody(new eRigidBody);
		SetModel(_model);
		SetName(_name);
	}
	ITerrainModel* getTerrainModel() { return static_cast<ITerrainModel*>(model.get()); }
};
#include "stdafx.h"

#include "BaseScript.h"
#include <base/Object.h>
#include <math/Transform.h>
#include <math/BoxCollider.h>

//---------------------------------------------------------------------------
eBaseScript::eBaseScript(Texture* flag_texture)
	: flag_tex(flag_texture)
	, flag(new eObject{})
{
  flag->SetTransform(new Transform);
}

eBaseScript::~eBaseScript()
{
	delete flag;
}

void eBaseScript::Update(std::vector<std::shared_ptr<eObject>> objs)
{
}




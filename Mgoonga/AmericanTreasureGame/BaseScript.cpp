#include "stdafx.h"

#include "BaseScript.h"
#include "Hex.h"

#include <base/Object.h>
#include <math/Transform.h>
#include <math/BoxCollider.h>

//---------------------------------------------------------------------------
eBaseScript::eBaseScript(IGame* _game, const Texture* flag_texture)
	: m_game(_game)
	, flag_tex(flag_texture)
	, flag(new eObject{})
{
  flag->SetTransform(new Transform);
}

//---------------------------------------------------------------------------
eBaseScript::~eBaseScript()
{
	delete flag;
}

//---------------------------------------------------------------------------
void eBaseScript::Update(float _tick)
{
	for (std::shared_ptr<eObject> other : m_game->GetObjects())
	{
		if (other->Name() != "Terrain"
			&& other->GetScript() != this
			&& std::find(m_objects_on_base.begin(), m_objects_on_base.end(), other) == m_objects_on_base.end()
			&& glm::length(object->GetTransform()->getTranslation() - other->GetTransform()->getTranslation()) < (Hex::radius))
		{
			m_objects_on_base.push_back(other);
			ObjectCameToBase.Occur(other, this->object->Name());
		}
	}
}




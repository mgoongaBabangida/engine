#ifndef SAND_BOX_SCRIPT_H
#define SAND_BOX_SCRIPT_H
#include "stdafx.h"

#include <base/interfaces.h>
#include <math/Clock.h>

//------------------------------------------------------------------
class eSandBoxScript : public IScript
{
public:
	explicit eSandBoxScript(const IGame*);
	virtual void	Update(float _tick) override;
	virtual void	CollisionCallback(const eCollision&);
protected:
	const IGame*	m_game;
	math::eClock	clock;
	float			g = 0.000098f;//per millisecond
};

#endif // SAND_BOX_SCRIPT_H
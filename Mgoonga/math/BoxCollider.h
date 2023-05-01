
#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H

#include <vector>

#include <base/interfaces.h>
#include <base/Object.h>
#include <math/PlaneLine.h>

#include <glm\glm/gtc/constants.hpp>

struct eCollision;

//----------------------------------------------------------------------------------
class DLL_MATH BoxCollider : public ICollider
{
public:
	virtual void CalculateExtremDots(const eObject* _object) override;
	virtual bool CollidesWith(const ITransform& trans,
														const ITransform& trans_other,
														const ICollider& other,
														Side moveDirection,
														eCollision& collision) override;
	virtual bool CollidesWith(const ITransform& trans,
														std::vector<shObject> objects,
														Side moveDirection,
														eCollision& collision) override;
	
	virtual glm::vec3								GetCenter() override;
	virtual std::vector<glm::mat3>	GetBoundingTriangles(const ITransform& trans)const override;
	virtual std::vector<glm::vec3>	GetExtrems(const ITransform& trans) const override;

	virtual std::vector<glm::mat3>	GetBoundingTrianglesLocalSpace()const override;
	virtual std::vector<glm::vec3>	GetExtremsLocalSpace() const override;
	virtual extremDots							GetExtremDotsLocalSpace() const override { return m_dots; }
protected:
	std::vector<dbb::line>	_getRays(const ITransform& trans, Side moveDirection, std::vector<float>& lengths);
	void										_getForwardRayLengths(const ITransform& trans,
																							 Side moveDirection,
																							 std::vector<float>& lengths)	const;

	extremDots m_dots;
	std::optional<glm::vec3> m_center = std::nullopt;
};

#endif
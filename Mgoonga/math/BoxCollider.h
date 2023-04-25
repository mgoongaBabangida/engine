
#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H

#include <vector>

#include <base/interfaces.h>
#include <math/PlaneLine.h>

#include <glm\glm/gtc/constants.hpp>

struct eCollision;

//----------------------------------------------------------------------------------
class DLL_MATH BoxCollider : public ICollider
{
public:
	virtual void CalculateExtremDots(const std::vector<glm::vec3>& positions) override;
	virtual bool CollidesWith(const ITransform& trans1,
														const ITransform& trans2,
														const ICollider& other,
														Side moveDirection, 
														eCollision& collision) override;
	
	virtual float getMaxX()const override { return dots.MaxX; }
	virtual float getMaxY()const override { return dots.MaxY; }
	virtual float getMaxZ()const override { return dots.MaxZ; }
	virtual float getMinX()const override { return dots.MinX; }
	virtual float getMinY()const override { return dots.MinY; }
	virtual float getMinZ()const override { return dots.MinZ; }
	
	virtual glm::vec3								GetCenter() override;
	virtual std::vector<glm::mat3>	GetBoundingTriangles(const ITransform& trans)const override;
	virtual std::vector<glm::vec3>	GetExtrems(const ITransform& trans) const override;

	virtual std::vector<glm::mat3>	GetBoundingTrianglesLocalSpace()const override;
	virtual std::vector<glm::vec3>	GetExtremsLocalSpace() const override;
protected:
	std::vector<dbb::line>	getRays(const ITransform& trans, Side moveDirection, std::vector<float>& lengths);
	void										getForwardRayLengths(const ITransform& trans,
																							 Side moveDirection,
																							 std::vector<float>& lengths)	const;

	extremDots dots;
	std::optional<glm::vec3> m_center = std::nullopt;
};

#endif
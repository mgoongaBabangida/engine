
#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H

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
	virtual float										GetRadius() override;
	virtual const std::string&			GetModelName() override { return m_model_name;  }
	virtual std::vector<glm::mat3>	GetBoundingTriangles(const ITransform& trans)const override;
	virtual std::vector<glm::vec3>	GetExtrems(const ITransform& trans) const override;

	virtual std::vector<glm::mat3>	GetBoundingTrianglesLocalSpace()const override;
	virtual std::vector<glm::vec3>	GetExtremsLocalSpace() const override;
	virtual extremDots							GetExtremDotsLocalSpace() const override { return m_dots; }

	virtual const std::string&			GetPath() const { return m_path; }
	virtual void										SetPath(const std::string& _path) { m_path = _path; }

protected:
	std::vector<dbb::line>	_getRays(const ITransform& trans, Side moveDirection, std::vector<float>& lengths);
	void										_getForwardRayLengths(const ITransform& trans,
																							 Side moveDirection,
																							 std::vector<float>& lengths)	const;
	bool										_CheckByRadius(const ITransform& _trans, const ITransform& _trans_other, ICollider* _other);

	extremDots								m_dots;
	std::optional<glm::vec3>	m_center = std::nullopt;
	float											m_radius = 0.0f;
	std::string								m_model_name;
	std::string								m_path;
	bool											m_check_sphere_overlap = false;
};

#endif
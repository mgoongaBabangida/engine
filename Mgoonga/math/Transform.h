#pragma once

#include "math.h"

#include <glm\glm\gtx\euler_angles.hpp>
#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>
#include <glm\glm\gtx\norm.hpp>
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include <glm\glm\gtc\constants.hpp>

#include <base\interfaces.h>

//--------------------------------------------------------------------------------------------
class DLL_MATH Transform : public ITransform
{
public:
	Transform(): 
		m_translation(0.0f, 0.0f, 0.0f)
		, m_scale(1.0f, 1.0f, 1.0f)
		, q_rotation(0.0f, 0.0f, 1.0f, 0.0f)
		, scaleIncrement(m_scale.x * 0.015f)
		{ UpdateModelMatrix();}
	
	glm::mat4			getModelMatrix() const;

	void				    setRotation(float x, float y, float z);
	void				    setRotation(glm::quat q)	 { q_rotation = q;		UpdateModelMatrix(); }
	virtual void		setTranslation(glm::vec3 tr) { m_translation = tr;	UpdateModelMatrix(); }
	virtual void		setScale(glm::vec3 sc) { m_scale = sc;	scaleIncrement = m_scale.x * 0.015f;	UpdateModelMatrix(); }
	
	bool				isRotationValid();

	static glm::quat	RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
	static Transform	interpolate(const Transform& first, const Transform& second, float progression);
	
	virtual glm::mat4	   getScale()			const { return  glm::scale(glm::vec3(m_scale.x, m_scale.y, m_scale.z)) ; }
  glm::vec3            getScaleAsVector() const { return m_scale; }
	virtual glm::vec3	   getTranslation()	const { return  m_translation; }
	glm::vec3&			     getTranslationRef()		  { return  m_translation; }
	virtual glm::quat	   getRotation()		const { return  q_rotation; }
	glm::vec4			       getRotationVector() const;
	glm::vec3			       getForward()		const { return forward; }
	glm::vec3			       getUp()				const { return Up; }
	
	void				incrementScale() { m_scale.x += scaleIncrement, m_scale.y += scaleIncrement, m_scale.z += scaleIncrement;  UpdateModelMatrix();}
	void				decrementScale() { m_scale.x -= scaleIncrement, m_scale.y -= scaleIncrement, m_scale.z -= scaleIncrement;  UpdateModelMatrix();}
	void				billboard(glm::vec3 direction);
	bool				turnTo(glm::vec3 dest, float speed);

protected:
	void				UpdateModelMatrix();

	glm::vec3			m_translation;
	glm::vec3			m_scale			= glm::vec3(1.0f, 1.0f, 1.0f);
	glm::quat			q_rotation		= glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3			forward			= glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3			Up				= glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4			totalTransform;
	float         scaleIncrement = 0.0f;
};
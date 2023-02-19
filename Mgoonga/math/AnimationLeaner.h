#pragma once

#include "math.h"

#include "stdafx.h"
#include "Clock.h"
#include <glm/glm/vec3.hpp>

namespace math
{
  //--------------------------------------------------------------------------------------
  class DLL_MATH AnimationLeaner
  {
  public:
    AnimationLeaner(std::vector<glm::vec3> start_points, std::vector<glm::vec3> end_points, int duration);
    
    void Start();
    std::vector<glm::vec3> getCurrentFrame();
    bool IsOn();
    void Reset();

  protected:
    std::vector<glm::vec3> m_start_points;
    std::vector<glm::vec3> m_end_points;
    math::eClock		m_clock;
    int					    m_duration; // msc
  };
}
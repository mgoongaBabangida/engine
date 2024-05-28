#pragma once

#include "math.h"

#include "stdafx.h"
#include "Clock.h"
#include <glm/glm/vec3.hpp>

namespace math
{
  //--------------------------------------------------------------------------------------
  template<class T>
  class AnimationLeaner
  {
  public:
    AnimationLeaner(std::vector<T> start_points, std::vector<T> end_points, int duration)
      :m_start_points(start_points), m_end_points(end_points), m_duration(duration)
    {
    }
    
    void Start()
    {
      m_clock.start();
    }

    std::vector<T> getCurrentFrame()
    {
      if (m_clock.timeElapsedMsc() > m_duration)
      {
        m_clock.pause();
        return m_end_points;
      }
      float time_stamp = m_clock.timeElapsedMsc() / static_cast<float>(m_duration);
      std::vector<T> res(m_start_points.size());
      for (size_t i = 0; i < m_start_points.size(); ++i)
      {
        res[i] = (m_end_points[i] - m_start_points[i]) * time_stamp + m_start_points[i];
      }
      return res;
    }

    bool IsOn()
    {
      return !m_clock.isPaused();
    }

    void Reset()
    {
      m_clock.reset();
    }

  protected:
    std::vector<T> m_start_points;
    std::vector<T> m_end_points;
    math::eClock		m_clock;
    int					    m_duration; // msc
  };
}
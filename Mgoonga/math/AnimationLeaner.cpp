#include "AnimationLeaner.h"

namespace math
{
  //----------------------------------------------------------
  AnimationLeaner::AnimationLeaner(std::vector<glm::vec3> start_points, std::vector<glm::vec3> end_points, int duration)
    :m_start_points(start_points), m_end_points(end_points), m_duration(duration)
  {
  }

  //----------------------------------------------------------
  void AnimationLeaner::Start()
  {
    m_clock.start();
  }

  //----------------------------------------------------------
  std::vector<glm::vec3> AnimationLeaner::getCurrentFrame()
  {
    if (m_clock.timeEllapsedMsc() > m_duration)
    {
      m_clock.pause();
      return m_end_points;
    }
    float time_stamp = m_clock.timeEllapsedMsc() / static_cast<float>(m_duration);
    std::vector<glm::vec3> res(m_start_points.size());
    for (size_t i = 0; i < m_start_points.size(); ++i)
    {
      res[i] = (m_end_points[i] - m_start_points[i]) * time_stamp + m_start_points[i];
    }
    return res;
  }

  //----------------------------------------------------------
  bool AnimationLeaner::IsOn()
  {
    return m_clock.isActive();
  }

  //----------------------------------------------------------
  void AnimationLeaner::Reset()
  {
    m_clock.reset();
  }
}

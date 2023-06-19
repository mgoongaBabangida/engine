#pragma once
#include <glm\glm\glm.hpp>

namespace dbb
{
  struct Rect
  {
    // this is for inverted y asix(from bottom to top) so its confusing need to improve
    bool IsInside(glm::vec2 _pos)
    {
      if (_pos.x >= m_top_left.x && (_pos.x - m_top_left.x) <= m_size.x
        && _pos.y <= m_top_left.y && (m_top_left.y - _pos.y) <= m_size.y)
        return true;
      else
        return false;
    }

    glm::vec2 m_top_left;
    glm::vec2 m_size;
  };
}

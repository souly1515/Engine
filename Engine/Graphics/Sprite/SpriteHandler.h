#pragma once

#include <vector>

#include "glm/glm/glm.hpp"

class SpriteShaderData
{
public:
  using SpritePos = glm::vec2;
  using SpriteSize = glm::vec2;
  SpriteSize m_spriteSize;
  SpritePos m_currentSpritePos;
  glm::vec4 m_hslOffset = glm::vec4{ 0,0,0,1 };
  glm::vec4 m_tint = glm::vec4{ 0,0,0,0 };
  float textureIndex = 0;
};
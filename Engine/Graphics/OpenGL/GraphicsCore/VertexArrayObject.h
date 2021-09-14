/******************************************************************************/
/*!
\file VertexArrayObject.h
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the header for VertexArrayObject.
VertexArrayObject is used to build basic meshes
*/
/******************************************************************************/
#pragma once
#include <vector>

#include "glew/glew.h"
#include "glm/glm/glm.hpp"

class VertexArrayObject
{
  uint16_t m_numVertices;
public:
  std::vector<float> m_elementArray;
  VertexArrayObject();
  uint16_t m_elementPerVertex;
 
  void PushValue(float num);

  void PushVertex(glm::vec3 pos, glm::vec4 color, glm::vec3 uv, glm::vec3 norm);

  void Clear();

  void SetNumberVertices(uint16_t numVertices);

  size_t GetSize() const;
  unsigned int GetNumberVertices() const;

  const float* GetVertexArray() const;

};


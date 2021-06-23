/******************************************************************************/
/*!
\file VertexArrayObject.cpp
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the implementation for VertexArrayObject.
VertexArrayObject is used to build basic meshes
*/
/******************************************************************************/
#include "VertexArrayObject.h"
#include <cassert>

VertexArrayObject::VertexArrayObject() : 
  m_numVertices{ 0 },
  m_elementPerVertex{ 0 }
{
}

void VertexArrayObject::PushValue(float num)
{
  m_elementArray.push_back(num);//position
}

void VertexArrayObject::PushVertex(glm::vec3 pos, glm::vec4 color, glm::vec3 uv)
{
  m_elementArray.push_back(pos.x);//position
  m_elementArray.push_back(pos.y);//position
  m_elementArray.push_back(pos.z);//position
  m_elementArray.push_back(color[0]);//color
  m_elementArray.push_back(color[1]);//color
  m_elementArray.push_back(color[2]);//color
  m_elementArray.push_back(color[3]);//color
  m_elementArray.push_back(uv.x);//u
  m_elementArray.push_back(uv.y);//v
  ++m_numVertices;
}

void VertexArrayObject::Clear()
{
  m_elementArray.clear();
}

void VertexArrayObject::SetNumberVertices(uint16_t numVertices) 
{
  assert(numVertices < m_elementArray.size());

  m_numVertices = numVertices;
}

size_t VertexArrayObject::GetSize() const
{
  return m_elementArray.size();
}

unsigned int VertexArrayObject::GetNumberVertices() const
{
  return m_numVertices;
}

const float* VertexArrayObject::GetVertexArray() const
{
  return m_elementArray.data();
}


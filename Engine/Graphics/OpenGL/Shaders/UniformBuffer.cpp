#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(size_t bindPoint):
  m_maxSize{ 0 },
  m_bindingPoint{ bindPoint }
{
  glGenBuffers(1, &m_bufferID);
}

UniformBuffer::~UniformBuffer()
{
  glDeleteBuffers(1, &m_bufferID);
}

void UniformBuffer::BindBuffer(shaderProg prog)
{
  unsigned int bufferIndex = glGetUniformBlockIndex(prog, m_name.c_str());
  glUniformBlockBinding(prog, bufferIndex, static_cast<GLuint>(m_bindingPoint));
  glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(m_bindingPoint), m_bufferID, 
    static_cast<GLuint>(m_bindingPoint), static_cast<GLuint>(m_maxSize));

}

void UniformBuffer::SetBufferData(GLvoid* data, size_t size)
{
  glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID);
  if (m_maxSize < size)
  {
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
    m_maxSize = size;
  }
  else
  {
    glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
  }
}

void UniformBuffer::SetBufferName(UniformName uniformName)
{
  m_name = uniformName;
}

UniformBuffer::UniformName UniformBuffer::GetBufferName()
{
  return m_name;
}

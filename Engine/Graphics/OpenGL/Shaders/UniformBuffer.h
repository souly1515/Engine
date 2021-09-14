#pragma once
#define GLEW_STATIC 
#include "glew/glew.h"
#include <string>

class UniformBuffer
{
public:
  using shaderProg = GLuint;
  using UniformName = std::string;
  using uboID = GLuint;

private:
  UniformName m_name;
  uboID m_bufferID;
  size_t m_maxSize;
  size_t m_bindingPoint;
public:

  UniformBuffer(size_t bindPoint);
  ~UniformBuffer();

  void BindBuffer(shaderProg prog);
  void SetBufferData(GLvoid* data, size_t size);
  
  void SetBufferName(UniformName uniformName);
  UniformName GetBufferName();
};

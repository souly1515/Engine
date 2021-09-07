/******************************************************************************/
/*!
\file BasicShader.h
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the declaration for BasicShader
Which is used to wrap shader code
*/
/******************************************************************************/
#pragma once

#define GLEW_STATIC // GLEW_STATIC to use the static version of the GLEW library

#include "glew\glew.h"//from http://glew.sourceforge.net/
#include <string>
#include <memory>
#include <vector>

#include "ShaderAttachment.h"
#include "UniformBuffer.h"
#include "glm/glm/glm.hpp"

struct ShaderProgram
{
private:

  GLuint m_vertexShaderId = { 0 };  // vertex shader Id
  GLuint m_fragmentShaderId = { 0 };  // fragment shader Id
  GLuint m_shaderProgramId = { 0 };

  std::string m_vertexSource;
  std::string m_fragmentSource;
  std::string m_geometrySource;

  std::string m_shaderName;
public:
  std::vector<std::shared_ptr<UniformBuffer>> m_UniformBufferStore;


  std::vector<ShaderAttachment> m_attachmentList;

  std::string m_vertexFile;
  std::string m_fragmentFile;

  ShaderProgram();

  bool Setup();//shaders code inside

  bool LoadShader();

  void Clean();

  void SetShaderName(std::string shaderName);
  std::string GetShaderName() const;

  GLuint GetProgramID() const;

  void RegisterAttachment(ShaderAttachment attachment);
  void ClearAttachments();

  void PushUniformBuffer(std::shared_ptr<UniformBuffer> buffer);
};

class BasicShader
{
protected:

  std::shared_ptr<ShaderProgram> shaderProg;

public:
  BasicShader();
  BasicShader(const BasicShader& rhs);
  BasicShader(std::shared_ptr<ShaderProgram> prog);
  ~BasicShader();

  void SetProgram(std::shared_ptr<ShaderProgram> prog);
  std::shared_ptr<ShaderProgram> GetProgram();

  bool operator== (const BasicShader& rhs) const;
  bool operator!= (const BasicShader& rhs) const;

  using ShaderPtr = std::shared_ptr<BasicShader>;

  GLuint GetProgramId() const;

  std::string GetShaderName() const;


  void SetVariables() const;


  void SetFloat(std::string name, float f) const;
  void SetVector2(std::string name, glm::vec2 vec) const;
  void SetVector3(std::string name, glm::vec3 vec) const;
  void SetVector4(std::string name, glm::vec4 vec) const;
  void SetInt(std::string name, int i) const;
  void SetMat(std::string name, glm::mat3 mat) const;
  void SetMat(std::string name, glm::mat4 mat) const;
  void SetFloatArray(std::string name, int arraySize, const float* arr) const;
  void SetIntArray(std::string name, int arraySize, const int* arr) const;

  void Start() const;
  void End() const;
};



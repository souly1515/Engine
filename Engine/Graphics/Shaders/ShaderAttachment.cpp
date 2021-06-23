/******************************************************************************/
/*!
\file ShaderAttachment.cpp
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the implementation for ShaderAttachment
Which is used to add extra shader code such as external functions,
geometry shader will probably be attached this way as well
*/
/******************************************************************************/
#include "ShaderAttachment.h"
#include <fstream>
#include <sstream>

ShaderAttachment::ShaderAttachment(ShaderType type, std::string filename):
  m_shaderID { 0 },
  m_type {type},
  m_source { },
  m_filename { SHADER_RELATIVE_PATH + filename }
{

}

GLuint ShaderAttachment::GetShaderID()
{
  return m_shaderID;
}

bool ShaderAttachment::Setup()
{
  GLenum glErr;
  GLint compiled = 0;
  glErr = glGetError(); // clear error cache
  GLuint oldProg = m_shaderID;

  if (m_filename != "")
  {
    std::ifstream fs;
    std::string source;
    std::stringstream ss;
    fs.open(m_filename);

    ss << fs.rdbuf();

    source = ss.str();

    m_source = source.c_str();

    fs.close();
  }

  const char* src = m_source.c_str();

  switch (m_type)
  {
  case ShaderType::E_FRAGMENT:
    m_shaderID = glCreateShader(GL_FRAGMENT_SHADER);

    break;
  case ShaderType::E_VERTEX:
    m_shaderID = glCreateShader(GL_VERTEX_SHADER);
    break;
  case ShaderType::E_GEOMETRY:
    m_shaderID = glCreateShader(GL_GEOMETRY_SHADER);
    break;
  default:
    break;
  }

  glErr = glGetError();
  if (glErr != GL_NO_ERROR)
  {
    //Log it
    std::string error = "Failed to create shader Error Code: ";
    error.append(std::to_string(glErr));
    //INF_CORE_ERROR(error);
    m_shaderID = oldProg;
    return false;
  }
  if (oldProg)
  {
    glDeleteShader(oldProg);
  }

  glShaderSource(m_shaderID, 1, &src, NULL);
  glCompileShader(m_shaderID);
  glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &compiled);

  if (!compiled)
  {
    GLchar infoLog[512];
    glGetShaderInfoLog(m_shaderID, 512, NULL, infoLog);

    std::string log("Shader Compile Error:\n");
    log += infoLog;
    //LOG it
    //INF_CORE_ERROR(log);
    return false;
  }

  return true;
}

void ShaderAttachment::Clean(GLuint programID)
{
  if (programID)
  {
    if (m_shaderID)
    {
      glDetachShader(programID, m_shaderID);
      glDeleteShader(m_shaderID);
    }
  }
}

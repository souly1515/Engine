/******************************************************************************/
/*!
\file BasicShader.cpp
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the implementation for BasicShader
Which is used to wrap shader code
*/
/******************************************************************************/
#include "BasicShader.h"

#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>

ShaderProgram::ShaderProgram():
  m_vertexSource{
  "#version 330 core  \n                                      \
      layout (location = 0) in vec3 position; // The position variable has attribute position 0 \n  \
      layout (location = 1) in vec4 color; // The color variable has attribute position 1  \n      \
      layout (location = 2) in vec2 aTexCoord;\n \
      out vec4 vertexColor; // Specify a color output to the fragment shader  \n            \
      out vec2 TexCoord; // Specify a color output to the fragment shader  \n            \
      uniform mat4 transform;\n\
      uniform mat4 worldTrans;\n\
      void main(void) { \n\
        mat4 transform2 = worldTrans * transform;\n\
        gl_Position = transform2 * vec4(position , 1.0f);\n    \
        vertexColor = color;\n            \
        TexCoord = aTexCoord;\n\
      }"
  },
  m_fragmentSource{
  "#version 330 core \n                                      \
      in vec4 vertexColor; \n                                     \
      in vec2 TexCoord; \n\
      uniform sampler2D ourTexture;\n \
      out vec4 color;\n                                         \
      void main(void) {\n                                         \
        vec4 tex = texture(ourTexture, TexCoord);\n                            \
        color = tex;  //decal color modulation  \n \
      }\n"
  }
{
}



BasicShader::BasicShader()
{
}

BasicShader::BasicShader(const BasicShader& rhs):
  shaderProg{ rhs.shaderProg }
{
}

BasicShader::BasicShader(std::shared_ptr<ShaderProgram> prog):
  shaderProg{ prog }
{
}


BasicShader::~BasicShader()
{
}

void BasicShader::SetProgram(std::shared_ptr<ShaderProgram> prog)
{
  shaderProg = prog;
}

std::shared_ptr<ShaderProgram> BasicShader::GetProgram()
{
    return shaderProg;
}

void ShaderProgram::RegisterAttachment(ShaderAttachment attachment)
{
  m_attachmentList.push_back(std::move(attachment));
}

void ShaderProgram::ClearAttachments()
{
  m_attachmentList.clear();
}

bool BasicShader::operator==(const BasicShader& rhs) const
{
  return shaderProg->GetProgramID() == rhs.shaderProg->GetProgramID();
}

bool BasicShader::operator!=(const BasicShader& rhs) const
{
  return !(*this == rhs);
}

GLuint BasicShader::GetProgramId() const
{
  return shaderProg->GetProgramID();
}


bool ShaderProgram::LoadShader()
{
  const char* vs_src = m_vertexSource.c_str();

  const char* fs_src = m_fragmentSource.c_str();


  GLint compiled = 0;
  // Create and compile vertex shader
  if(m_vertexShaderId)
    glDeleteShader(m_vertexShaderId);
  m_vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(m_vertexShaderId, 1, &vs_src, NULL);
  glCompileShader(m_vertexShaderId);
  glGetShaderiv(m_vertexShaderId, GL_COMPILE_STATUS, &compiled);

  if (!compiled)
  {
    GLchar infoLog[512];
    glGetShaderInfoLog(m_vertexShaderId, 512, NULL, infoLog);
    std::string log("Vertex Shader Compile Error:\n");
    log += infoLog;

    //LOG it
    //INF_CORE_ERROR(log);
		assert(false);
    return false;
  }

  compiled = 0;

  // Create and compile fragment shader
  if(m_fragmentShaderId)
    glDeleteShader(m_fragmentShaderId);

  m_fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(m_fragmentShaderId, 1, &fs_src, NULL);
  glCompileShader(m_fragmentShaderId);
  glGetShaderiv(m_fragmentShaderId, GL_COMPILE_STATUS, &compiled);

  if (!compiled)
  {
    GLchar infoLog[512];
    glGetShaderInfoLog(m_fragmentShaderId, 512, NULL, infoLog);
    std::string log("Fragment Shader Compile Error:\n");
    log += infoLog;
    //LOG it
    //INF_CORE_ERROR(log);
		assert(false);
    return false;
  }

  glAttachShader(m_shaderProgramId, m_vertexShaderId);
  glAttachShader(m_shaderProgramId, m_fragmentShaderId);

  for (auto& attachment : m_attachmentList)
  {
    attachment.Setup();
    glAttachShader(m_shaderProgramId, attachment.GetShaderID());
  }

  GLint linked = 0;
  glLinkProgram(m_shaderProgramId);
  glGetProgramiv(m_shaderProgramId, GL_LINK_STATUS, &linked);

  if (!linked)
  {
    GLint  length;
    glGetProgramiv(m_shaderProgramId, GL_INFO_LOG_LENGTH, &length);
    std::vector<GLchar> infoLog(length);
    glGetProgramInfoLog(m_shaderProgramId, length, &length, &infoLog[0]);
    std::string log("Shader Link Error:\n");
    log += &infoLog[0];
    //LOG it
		//std::cout << (log) << std::endl;
		assert(false);
    return false;
  }

  return compiled;
}



std::string BasicShader::GetShaderName() const
{
  return shaderProg->GetShaderName();
}

bool ShaderProgram::Setup()
{
  GLenum glErr;
  glErr = glGetError(); // clear error cache
  GLuint oldProg = m_shaderProgramId;
  // Create prog, attach shaders, and link it
  m_shaderProgramId = glCreateProgram();

  std::string source;
  std::stringstream ss;
  if (m_vertexFile != "")
  {
		std::ifstream fs(m_vertexFile);

		if (fs.good())
		{
			ss << fs.rdbuf();

			source = ss.str();

			m_vertexSource = (source.c_str());

			fs.close();
			ss.str("");
		}
		else
			assert(false);
  }

  if (m_fragmentFile != "")
  {
		std::ifstream fs(m_fragmentFile);
    
    if (fs.good())
    {
      ss << fs.rdbuf();

      source = ss.str();

      m_fragmentSource = (source.c_str());
      fs.close();
    }
    else
    {
      assert(false);
    }
  }
  glErr = glGetError();
  if (glErr != GL_NO_ERROR)
  {
    //Log it
    /*
    std::string error = "Failed to create shader Error Code: ";
    error.append(std::to_string(glErr));
    INF_CORE_ERROR(error);
    */
    m_shaderProgramId = oldProg;
		assert(false);
    return false;
  }
  if (glIsProgram(oldProg))
  {
    glDeleteProgram(oldProg);
  }
  return LoadShader();
}

void ShaderProgram::Clean()
{
  if (m_shaderProgramId)
  {
    if (m_vertexShaderId)
    {
      glDetachShader(m_shaderProgramId, m_vertexShaderId);
      glDeleteShader(m_vertexShaderId);
    }

    if (m_fragmentShaderId)
    {
      glDetachShader(m_shaderProgramId, m_fragmentShaderId);

      glDeleteShader(m_fragmentShaderId);
    }

    glDeleteShader(m_shaderProgramId);
  }
}

void ShaderProgram::SetShaderName(std::string shaderName)
{
  m_shaderName = shaderName;
}

std::string ShaderProgram::GetShaderName() const
{
  return m_shaderName;
}

GLuint ShaderProgram::GetProgramID() const
{
  return m_shaderProgramId;
}

void BasicShader::SetVariables() const
{
}

void ShaderProgram::PushUniformBuffer(std::shared_ptr<UniformBuffer> buffer)
{
  m_UniformBufferStore.push_back(buffer);
}

void BasicShader::SetFloat(std::string name, float f) const
{
  glUniform1f(glGetUniformLocation(shaderProg->GetProgramID(), name.c_str()), f);
}

void BasicShader::SetVector2(std::string name, glm::vec2 vec) const
{
  glUniform2f(glGetUniformLocation(shaderProg->GetProgramID(), name.c_str()), vec[0], vec[1]);
}

void BasicShader::SetVector3(std::string name, glm::vec3 vec) const
{
  glUniform3f(glGetUniformLocation(shaderProg->GetProgramID(), name.c_str()), vec[0], vec[1], vec[2]);
}

void BasicShader::SetVector4(std::string name, glm::vec4 vec) const
{
  glUniform4f(glGetUniformLocation(shaderProg->GetProgramID(), name.c_str()), vec[0], vec[1], vec[2], vec[3]);
}

void BasicShader::SetInt(std::string name, int i) const
{
  glUniform1i(glGetUniformLocation(shaderProg->GetProgramID(), name.c_str()), i);
}

void BasicShader::SetMat(std::string name, glm::mat3 mat) const
{
  if (!shaderProg->GetProgramID())
    return;
  GLint loc = glGetUniformLocation(shaderProg->GetProgramID(), name.c_str());
  if (loc == -1)
    return;

  glProgramUniformMatrix3fv(shaderProg->GetProgramID(), loc, 1, GL_FALSE, &mat[0][0]);
}

void BasicShader::SetMat(std::string name, glm::mat4 mat) const
{
  if (!shaderProg->GetProgramID())
    return;
  GLint loc = glGetUniformLocation(shaderProg->GetProgramID(), name.c_str());
  if (loc == -1)
    return;
  glProgramUniformMatrix4fv(shaderProg->GetProgramID(), loc, 1, GL_FALSE, &mat[0][0]);
}

void BasicShader::SetFloatArray(std::string name, int arraySize, const float* arr) const
{
  glProgramUniform1fv(shaderProg->GetProgramID(),
    glGetUniformLocation(shaderProg->GetProgramID(), name.c_str()), arraySize, arr);
}

void BasicShader::SetIntArray(std::string name, int arraySize, const int* arr) const
{
  glProgramUniform1iv(shaderProg->GetProgramID(),
    glGetUniformLocation(shaderProg->GetProgramID(), name.c_str()), arraySize, arr);
}

void BasicShader::Start() const
{
  glUseProgram(shaderProg->GetProgramID());
}


void BasicShader::End() const
{
  glUseProgram(0);
}
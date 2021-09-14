/******************************************************************************/
/*!
\file ShaderAttachment.h
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the declarations for ShaderAttachment
Which is used to add extra shader code such as external functions,
geometry shader will probably be attached this way as well
*/
/******************************************************************************/
#pragma once
#include <string>

#define SHADER_RELATIVE_PATH ""
// remove if we are switching to DLL
#define GLEW_STATIC 
#include "glew/glew.h"

enum class ShaderType
{
  E_FRAGMENT,
  E_VERTEX,
  E_GEOMETRY
};

class ShaderAttachment
{
  GLuint m_shaderID;
  ShaderType m_type;
  std::string m_source;
  std::string m_filename;

public:
  ShaderAttachment(ShaderType type, std::string filename);
  GLuint GetShaderID();
  bool Setup();//shaders code inside
  void Clean(GLuint programID);


};


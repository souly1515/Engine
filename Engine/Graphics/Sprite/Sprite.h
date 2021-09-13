/******************************************************************************/
/*!
\file Sprite.h
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the declaration for sprite class and wireframes
which is used to hold and draw objects
*/
/******************************************************************************/
#pragma once

// remove if we are switching to DLL
#define GLEW_STATIC 
#include <memory>

#include "Graphics/Mesh/Mesh.h"
#include "Graphics/Shaders/BasicShader.h"
#include "Graphics/GraphicSystem.h"
#include "glm/glm/glm.hpp"
#include <functional>

#include "SpriteHandler.h"
#include <string>

class Sprite
{
public:
  using TextureID = GLuint;
//protected:
  BasicShader::ShaderPtr m_shader;
  std::shared_ptr<Mesh> m_mesh;
  
  TextureID m_textureID;
public:
  //SpriteShaderData m_shaderData;

  Sprite() = default;
  Sprite(Sprite && rhs) = default;
  Sprite(const Sprite & rhs) = default;
  Sprite(BasicShader::ShaderPtr shader,
    std::shared_ptr<Mesh> mesh, TextureID textureID);
  Sprite(std::shared_ptr<Sprite> sprite);
  ~Sprite();

  Sprite& operator=(Sprite&& rhs) = default;
  Sprite& operator=(const Sprite & rhs) = default;

  void Draw();
  void SetShader();

  BasicShader::ShaderPtr GetShader() const;
  std::string GetShaderName() const;

  void SetShader(BasicShader::ShaderPtr shader);
  void SetShader(std::string shader);

  TextureID GetTextureID() const;

  void SetTexture(TextureID TexID);

  std::shared_ptr<Mesh> GetMesh() const;

  void Validate()
  {
    if (m_shader)
      m_shader->GetProgram()->GetProgramID();
    if (m_mesh)
      *m_mesh;
  }

  std::string GetLogData()
  {
    return "m_shader->program use count: " + std::to_string(m_shader->GetProgram().use_count()) + "\nm_shaderuse count: " +
      std::to_string(m_shader.use_count()) + "\n";
  }

};

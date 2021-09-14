/******************************************************************************/
/*!
\file Sprite.cpp
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the implementaion for sprite class and wireframes
which is used to hold and draw objects
*/
/******************************************************************************/
#include "Sprite.h"
#include "../GraphicSystem.h"


Sprite::Sprite(BasicShader::ShaderPtr shader,
  std::shared_ptr<Mesh> mesh, Sprite::TextureID textureID):
  m_shader{ shader },
  m_mesh{ mesh },
  m_textureID { textureID }
{
}

Sprite::Sprite(std::shared_ptr<Sprite> sprite) : Sprite(*sprite)
{
}

Sprite::~Sprite()
{
  m_shader.reset();
  m_mesh.reset();
}

void Sprite::Draw()
{
  //glBindTexture(GL_TEXTURE_2D, m_textureID);
  glBindVertexArray(m_mesh->VAOref);
  glDrawElements(m_mesh->drawMode, static_cast<GLsizei>(m_mesh->IA.size()), GL_UNSIGNED_INT, 0);
}

void Sprite::SetShader()
{
  m_shader->Start();
}

BasicShader::ShaderPtr Sprite::GetShader() const
{
	return m_shader;
}

std::string Sprite::GetShaderName() const
{
  return GraphicsSystem_OpenGL::GetInstance()->ShaderMan.GetName(m_shader);
}

void Sprite::SetShader(BasicShader::ShaderPtr shader)
{
  m_shader= shader;
}

void Sprite::SetShader(std::string shader)
{
  SetShader(GraphicsSystem_OpenGL::GetInstance()->ShaderMan.GetShader(shader));
}

typename Sprite::TextureID Sprite::GetTextureID() const
{
  return m_textureID;
}


void Sprite::SetTexture(TextureID TexID)
{
  m_textureID = TexID;
}

std::shared_ptr<Mesh> Sprite::GetMesh() const
{
  return m_mesh;
}

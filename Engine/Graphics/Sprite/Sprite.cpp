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
#include "Graphics/GraphicSystem.h"


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
}

void Sprite::Draw()
{
  /*
  if (m_shader)
  {
    m_shader->Start();

    //TransformComponent& trans = ecs.GetComponent<TransformComponent>(entity);
    m_shader->SetMat("transform", trans.GetMatrix());
    m_shader->SetFloat("rotation", trans._rotationX);
    m_shader->SetMat("worldTrans", GraphicSystem::GetWorldTrans());
    //m_shader->SetInt("curPosX", m_currentSpritePos.x());
    //m_shader->SetInt("curPosY", m_currentSpritePos.y());
    //m_shader->SetInt("TextureSizeX", m_spriteSize.x());
    //m_shader->SetInt("TextureSizeY", m_spriteSize.y());
    //trans._position.z() = trans._position.y();
    m_shader->SetVariables();

  }
  */
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
  return GraphicSystem::GetInstance()->ShaderMan.GetName(m_shader);
}

void Sprite::SetShader(BasicShader::ShaderPtr shader)
{
  m_shader= shader;
}

void Sprite::SetShader(std::string shader)
{
  SetShader(GraphicSystem::GetInstance()->ShaderMan.GetShader(shader));
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

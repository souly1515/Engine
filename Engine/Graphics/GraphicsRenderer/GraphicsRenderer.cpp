/******************************************************************************/
/*!
\file GraphicsRenderer.cpp
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the implementation of graphics renderer
which is used to draw entities with sprite components
*/
/******************************************************************************/
#include "GraphicsRenderer.h"
#include "glm/glm/glm.hpp"

GraphicsRendererSystem::GraphicsRendererSystem():
  m_gs { GraphicSystem::GetInstance() }
{
  //SubscribeEvent(&GraphicsRendererSystem::HandleKeyDown);
}

GraphicsRendererSystem::~GraphicsRendererSystem()
{
  //m_gs.Exit();
}

void GraphicsRendererSystem::Init()
{
}


void GraphicsRendererSystem::Update()
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GraphicsRendererSystem::Exit()
{
  GraphicSystem::Exit();
}


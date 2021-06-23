/******************************************************************************/
/*!
\file ShaderReset.cpp
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the implementation for a class that will reset shaders 
within the graphics system
*/
/******************************************************************************/
#include "ShaderReset.h"


GraphicSystem* ShaderReset::gs = nullptr;

void ShaderReset::ResetShader()
{
  gs = GraphicSystem::GetInstance();
  gs->ShaderMan.ReloadShaders();
}

void ShaderReset::SetGraphicSystem(GraphicSystem* ngs)
{
  gs = ngs;
}

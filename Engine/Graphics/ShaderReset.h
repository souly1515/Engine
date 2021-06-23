/******************************************************************************/
/*!
\file ShaderReset.h
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the declaration for a class that will reset shaders
within the graphics system
*/
/******************************************************************************/
#include "GraphicSystem.h"

class ShaderReset
{
private:
  static GraphicSystem* gs;
public:
  static void ResetShader();
  static void SetGraphicSystem(GraphicSystem* ngs);
};

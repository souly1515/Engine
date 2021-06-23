/******************************************************************************/
/*!
\file GraphicsRenderer.h
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the declaration of graphics renderer
which is used to draw entities with sprite components
*/
/******************************************************************************/
#pragma once
#include <memory>
#include "Graphics/GraphicSystem.h"

#include "Graphics/Sprite/Sprite.h"

class GraphicsRendererSystem
{
private:
  GraphicSystem* m_gs;
public:
  GraphicsRendererSystem();
  ~GraphicsRendererSystem();

  void Init();
  void Update();
  void Exit();
};


/******************************************************************************/
/*!
\file Mesh.h
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the struct Mesh which holds all the data relevent to building a mesh
*/
/******************************************************************************/
#pragma once

#include "../GraphicsCore/VertexArrayObject.h"

struct Mesh
{
  VertexArrayObject VAO;
  std::vector<unsigned int> IA;
  GLuint VAOref;
  GLuint VBOref;
  GLuint EBOref;
  GLenum drawMode;
  void Clear();
  Mesh() = default;
};
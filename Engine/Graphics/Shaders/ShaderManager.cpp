/******************************************************************************/
/*!
\file ShaderManager.cpp
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the definitions for ShaderManager
Which is meant to handle shader management, such as distributing of shaders from a shared pool
*/
/******************************************************************************/
#include "Graphics/GraphicSystem.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include "Graphics/Sprite/Sprite.h"
#include "BasicShader.h"



using ShaderPtr = GraphicSystem::ShaderPtr;
using ShaderName = GraphicSystem::ShaderName;
using Filename = GraphicSystem::Filename;

ShaderPtr GraphicSystem::ShaderManager::GetShader(ShaderName name) const
{
  try
  {
    auto temp = std::make_shared<BasicShader>(m_shaderMap.at(name));
    return temp;
  }
  catch (std::out_of_range)
  {
    return ShaderPtr{};
  }
}

ShaderPtr GraphicSystem::ShaderManager::LoadShader(Filename vertexSource,
  Filename fragmentSource, ShaderName name, std::vector<ShaderAttachment> attachmentList)
{
  LoadShaderNoRet(vertexSource, fragmentSource, name, attachmentList);
  return std::make_shared<BasicShader>(m_shaderMap.at(name));
}

void GraphicSystem::ShaderManager::LoadShaderNoRet(Filename vertexSource, 
  Filename fragmentSource, ShaderName name, std::vector<ShaderAttachment> attachmentList)
{
  ShaderProg shader;
  try
  {
    shader = m_shaderMap.at(name);
  }
  catch (std::out_of_range)
  {
    shader = std::make_shared<ShaderProgram>();
  }
	if(vertexSource != "")
	  shader->m_vertexFile = (SHADER_RELATIVE_PATH + vertexSource).c_str();
	else
		shader->m_vertexFile = "";
	if (fragmentSource != "")
	  shader->m_fragmentFile = (SHADER_RELATIVE_PATH + fragmentSource).c_str();
	else
		shader->m_fragmentFile = "";
  
  for (ShaderAttachment attachment : attachmentList)
  {
    shader->RegisterAttachment(attachment);
  }

  // if shader fails then so be it
  if (shader->Setup())
  {
    std::string temp;
    temp = name;
    temp += " Loaded";
    //INF_CORE_INFO(temp);
  }

  shader->SetShaderName(name);

  RegisterShader(shader, name);
}


void GraphicSystem::ShaderManager::RegisterShader(ShaderProg shader, ShaderName name)
{
  m_shaderMap[name] = shader;
  m_reverseMap[shader] = name;
}


GraphicSystem::ShaderManager::~ShaderManager()
{
  std::for_each(begin(m_shaderMap), end(m_shaderMap), [](std::pair<ShaderName, ShaderProg> shader)
    {
      shader.second->Clean();
      shader.second.reset();
    });
}

void GraphicSystem::ShaderManager::ReloadShaders()
{
  std::for_each(std::begin(m_shaderMap), std::end(m_shaderMap),
    [](std::pair<ShaderName, ShaderProg> shader)
    {
      if (shader.second->Setup())
      {
        std::string log = "built shader ";
        log += shader.first;
        //INF_CORE_INFO(log);
      }
    });
}

void GraphicSystem::ShaderManager::CreateUniformBuffer(UniformBuffer::UniformName name)
{
  std::shared_ptr<UniformBuffer> buffer;
  buffer = std::make_shared<UniformBuffer>(m_NextBindPoint);
  ++m_NextBindPoint;
  buffer->SetBufferName(name);
  m_uniformBufferMap[name] = buffer;
}


void GraphicSystem::ShaderManager::InitBasicShaders()
{
  //LoadShaderNoRet("BatchVertexShader.glsl", "BatchFragmentShader.glsl", "default");
}

void GraphicSystem::ShaderManager::SetUniformBufferData(UniformBuffer::UniformName name, GLvoid* data, size_t size)
{
  auto itr = m_uniformBufferMap.find(name);
  if (itr != m_uniformBufferMap.end())
  {
    itr->second->SetBufferData(data, size);
  }
  else
  {
    std::string warning = "no uniform buffer " + name + " found";
    //INF_CORE_WARN(warning.c_str());
  }
}

std::string  GraphicSystem::ShaderManager::GetName(ShaderPtr shader)
{
  auto itr = std::find_if(std::begin(m_reverseMap), std::end(m_reverseMap), [&shader](auto it)
    {
      return ((static_cast<ShaderProg>(it.first)->GetProgramID()) == shader->GetProgramId());
    });
  if(itr != m_reverseMap.end())
    return itr->second;
  return "";
}


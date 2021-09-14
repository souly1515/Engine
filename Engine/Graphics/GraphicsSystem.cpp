#include "GraphicsSystem.h"


#ifdef USE_VULKAN

#else
#include "OpenGL/GraphicSystem.h"
#endif


GraphicsSystem* GraphicsSystem::m_gs = nullptr;

GraphicsSystem* GraphicsSystem::GetInstance()
{
  if (!m_gs)
  {
#ifdef USE_VULKAN

#else
    m_gs = new GraphicsSystem_OpenGL;
#endif
  }
  return  m_gs;
}

void GraphicsSystem::Exit()
{
  delete m_gs;
  m_gs = nullptr;
}

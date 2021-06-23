#include "EngineManager.h"
#include "Graphics/WinWrapper.h"
#include "Graphics/GraphicSystem.h"
#include "Graphics/Sprite/Sprite.h"
#include <chrono>

Entity Engine::EngineManager::CloneEntity(Entity entity)
{
	return EntMan.CloneEntity(entity);
}

#include <iostream>
void Engine::EngineManager::Run()
{
  WinWrapper* winp = WinWrapper::GetInstance();
  auto last = std::chrono::system_clock::now();
  while (winp->ManageMessage())
  {
    std::chrono::duration<float> dur = std::chrono::system_clock::now() - last;
    while (dur.count() < 0.016)
    {
      dur = std::chrono::system_clock::now() - last;
    }
    SysMan.Run(EntMan);
    last = std::chrono::system_clock::now();
  }
}

Engine::EngineManager::~EngineManager()
{
  GraphicSystem::Exit();
  WinWrapper::Exit();
}

void Engine::EngineManager::RunSystemOnce()
{
  SysMan.Run(EntMan);
}

void Engine::EngineManager::Init(_In_ HINSTANCE hInstance,
  _In_ int       nCmdShow)
{
  WinWrapper* winp = WinWrapper::GetInstance();
  //winp->wndProcPtr = ImGui_ImplWin32_WndProcHandler;

#ifndef INF_DIST
  [[maybe_unused]] bool alloc = true;
#else 
  [[maybe_unused]] bool alloc = false;
#endif 

  winp->SetupWindows(hInstance, nCmdShow, alloc);
  SetWindowLongPtr(winp->GetInstance()->GetHWND(), GWLP_WNDPROC, (LONG_PTR)winp->GetInstance()->wndProcPtr);

  GraphicSystem* gs = GraphicSystem::GetInstance();
}

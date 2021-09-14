/******************************************************************************/
/*!
\file WinWrapper.cpp
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This is the Definitions for Winwrapper
Which handles windows related APIs
*/
/******************************************************************************/
#include "WinWrapper.h"
#include <iostream>
#include <Windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>



//#include "Core/Log.h"

#ifndef MAX_LOADSTRING
#define MAX_LOADSTRING 100
#endif

#define RESOLUTION_FILE "../Data/Config/Resolution.json"

//#define RESOLUTION_FILE "..\\..\\..\\Data\\Config\\Resolution.json"

WinWrapper* WinWrapper::instance = nullptr;

void WinWrapper::SetupWindows(HINSTANCE nhInstance, int nnCmdShow, bool ShowConsole)
{
  this->hInstance = nhInstance;
  this->nCmdShow = nnCmdShow;

  LoadStringW(hInstance, 101, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, 101, szWindowClass, MAX_LOADSTRING);

  /*std::string string{ "MadLads" };

  std::wstring wstring{ std::begin(string), std::end(string) };

  szTitle =
  szWindowClass = wstring.c_str();*/
  auto res = MyRegisterClass();
  if (!res)
  {
    auto errorcode = GetLastError();
    assert(res);
  }
  InitInstance(ShowConsole);

  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(101));
}

bool WinWrapper::ManageMessage()
{
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    if (msg.message == WM_QUIT)
      return  false;

    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  return true;
}

WinWrapper * WinWrapper::GetInstance()
{
  if (!instance)
    instance = new WinWrapper();
  return instance;
}
HWND WinWrapper::GetHWND()
{
  return instance->mainHWND;
}


BOOL WinWrapper::InitInstance(bool ShowConsole)
{
  //WS_CLIPCHILDREN: Excludes the area occupied by child windows when drawing occurs within the parent window
  //WS_CLIPSIBLINGS: Same for child windows - relative to each other.

  //LoadStringW(hInstance, 101, szTitle, MAX_LOADSTRING);
  //LoadStringW(hInstance, 101, szWindowClass, MAX_LOADSTRING);
  DWORD dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;
  //DWORD dwStyle = WS_OVERLAPPEDWINDOW;
  dwStyle &= ~WS_SIZEBOX;
  dwStyle &= ~WS_MAXIMIZEBOX;

  RECT rect = { 0, 0, (LONG)(windowWidth - 1), (LONG)(windowHeight - 1) };
  //The AdjustWindowRect sets the exact client area without the title bar and all the extra pixels
  //This will give us the exact resolution for the white rectangular area
  AdjustWindowRectEx(&rect, dwStyle, FALSE, WS_EX_APPWINDOW);
  


  mainHWND = 
    CreateWindowExW(WS_EX_APPWINDOW, szWindowClass, L"Engine", dwStyle,
    CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);
    //CreateWindowW(szWindowClass, L"MadLads!", dwStyle,
    //CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

  if (!mainHWND)
  {
    //INF_ASSERT(false, "Unable to create window");

    auto error = GetLastError();

    assert(mainHWND);
    return FALSE;
  }

  if (ShowConsole)
  {
    AllocConsole();
    
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    std::cout.clear();
    consoleHWND = GetConsoleWindow();
    if (consoleHWND == NULL)
    {
      //INF_ASSERT(false, "Unable to retrieve console window");
      return FALSE;
    }

    // Disabling the close button
    consoleHWND = GetConsoleWindow();
    HMENU hMenu = GetSystemMenu(consoleHWND, FALSE);
    DeleteMenu(hMenu, 6, MF_BYPOSITION);

  }

  //SetWindowText(mainHWND, std::w_string("MadLads" ));
  ShowWindow(mainHWND, nCmdShow);
  UpdateWindow(mainHWND);
  // Accept files dragged in
  DragAcceptFiles(mainHWND, true);

  return TRUE;
}
ATOM WinWrapper::MyRegisterClass()
{
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = NULL;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(101);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = NULL;

  return RegisterClassExW(&wcex);
}

LRESULT WinWrapper::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {

  case WM_PAINT:
  {
    PAINTSTRUCT ps;
    /*HDC hdc = */BeginPaint(hWnd, &ps);
    // TODO: Add any drawing code that uses hdc here...
    EndPaint(hWnd, &ps);
  }
  break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  // As long as some input is detected, update key press
  // Input detection: Mouse & Keyboard
  case WM_KEYDOWN:
  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
  case WM_MBUTTONDOWN:
    //INF_Input.KeyPressFromWndProc(wParam);
    break;
  // Input release: Mouse & Keyboard
  case WM_KEYUP:
  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
  case WM_MBUTTONUP:
    //INF_Input.KeyReleaseFromWndProc(wParam);
    break;
  case WM_MOUSEWHEEL:
    //INF_Input.UpdateScrollDelta(wParam);
    break;
  /*case WM_SIZE:
  {
    unsigned int width = static_cast<unsigned int>(LOWORD(lParam));
    unsigned int height = static_cast<unsigned int>(HIWORD(lParam));

    INF::EventManager::SendEvent<Windows::ResizeEvent>(width, height);
  }
   break;*/

  case WM_DROPFILES:
    //Infinity::MyEditor::GetInstance()->DragAndDrop(wParam);
    break;
  default:
    if (instance && instance->InputHandle)
    {
      instance->InputHandle(message);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

HWND WinWrapper::GetConsoleHWND()
{
  return instance->consoleHWND;
}

unsigned int WinWrapper::GetWindowWidth()
{
  return instance->windowWidth;
}

unsigned int WinWrapper::GetWindowHeight()
{
  return instance->windowHeight;
}

void WinWrapper::Exit()
{
  if (instance)
  {
    delete instance;
    instance = nullptr;
  }
}

WinWrapper::WinWrapper():
  windowWidth{ 1280 },
  windowHeight{ 720 },
  InputHandle {},
  msg {},
  hInstance {},
  mainHWND {},
  szTitle {},
  szWindowClass {},
  nCmdShow {}
{
 /* Serializer seri{ RESOLUTION_FILE };
  if (seri.Exist())
  {
    seri.UpdateObject(*this);
  }*/
}


WinWrapper::~WinWrapper()
{
  /*Serializer seri{ RESOLUTION_FILE };
  seri.StartSerialize();
  seri.SerializeObject(*instance);
  seri.EndSerialize();*/
}



void WinWrapper::SetWindowWidth(unsigned int width)
{
  windowWidth = width;
}

void WinWrapper::SetWindowHeight(unsigned int height)
{
  windowHeight = height;
}

void WinWrapper::DestroyWindow()
{
  ::DestroyWindow(this->GetHWND());
}

void WinWrapper::UpdateWindowSize()
{
  ::SetWindowPos(GetHWND(), 0, 0, 0, (LONG)(windowWidth - 1), (LONG)(windowHeight - 1), SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}
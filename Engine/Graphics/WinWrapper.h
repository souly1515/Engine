/******************************************************************************/
/*!
\file WinWrapper.h
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
#pragma once

#define NOMINMAX
#include <Windows.h>

class WinWrapper
{
public:
  void SetupWindows(HINSTANCE hInstance, int nCmdShow, bool ShowConsole = true);
  bool ManageMessage();
  static WinWrapper* GetInstance();
  static HWND GetHWND();
  static unsigned int GetWindowWidth();
  static unsigned int GetWindowHeight();
  static void Exit();

  void SetWindowWidth(unsigned int width);
  void SetWindowHeight(unsigned int height);
  void UpdateWindowSize();
  LRESULT(*wndProcPtr)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = nullptr;
  RECT winRECT;
  int (*InputHandle)(UINT msg);
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  static HWND GetConsoleHWND();
  void DestroyWindow();
private:
  
  BOOL InitInstance(bool ShowConsole = true);
  ATOM MyRegisterClass();

  
  unsigned int windowWidth;
  unsigned int windowHeight;
  
  MSG msg;
  HINSTANCE hInstance;
  HWND mainHWND;
  HWND consoleHWND;

  HACCEL hAccelTable;
  WCHAR szTitle[100];                  // The title bar text
  WCHAR szWindowClass[100];            // the main window class name
  int nCmdShow;
  static WinWrapper* instance;
  WinWrapper();
  ~WinWrapper();
};

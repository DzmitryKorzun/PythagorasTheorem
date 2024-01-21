#pragma once
#include <windows.h>;
#include "IDX.h";

class Window
{
public:
	Window();
	static int RUN(IDX* idxGame, HINSTANCE hInstance, int nCmdShow);
	static HWND GetHwnd() { return m_hWnd; }
	bool Init();
	bool BroadCast();
	bool IsWindowRunning();
	bool Realise();

	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void OnUpdate();

	~Window();

protected:
	static HWND m_hWnd;
	bool isRunning;
};


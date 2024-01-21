#pragma once
#include <windows.h>;

class Window
{
public:
	Window();
	bool Init();
	bool BroadCast();
	bool IsWindowRunning();
	bool Realise();

	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void OnUpdate();

	~Window();

protected:
	HWND hWnd;
	bool isRunning;
};


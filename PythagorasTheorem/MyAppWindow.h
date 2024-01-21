#pragma once
#include "Window.h"
class MyAppWindow : public Window
{
public:
	virtual void OnCreate() override;
	virtual void OnUpdate() override;
	virtual void OnDestroy() override;
};
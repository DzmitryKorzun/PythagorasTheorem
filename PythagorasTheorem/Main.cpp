#include "MyAppWindow.h"

int main()
{
	MyAppWindow application;
	if (application.Init())
	{
		while (application.IsWindowRunning())
		{
			application.BroadCast();
		}
	}
	return 0;	
}

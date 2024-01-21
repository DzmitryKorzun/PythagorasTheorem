#include "MyAppWindow.h"
#include "Game.h"
int main()
{
	Game game(800, 600, L"lalala");
	return Window::RUN(&game, nullptr, SW_SHOW);
}

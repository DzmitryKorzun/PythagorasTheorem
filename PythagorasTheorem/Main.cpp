#include "MyAppWindow.h"
#include "Game.h"
int main()
{
	Game game(800, 600, L"PythagorasTheorem");
	return Window::RUN(&game, nullptr, SW_SHOW);
}

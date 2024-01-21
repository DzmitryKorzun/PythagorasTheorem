#include "Window.h"
#include "stdexcept"
#include <d3d12.h>
#include <dxgi1_6.h>

Window::Window()
{
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        Window* window = (Window*)((LPCREATESTRUCT)lParam)->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)
            window);
        window->OnCreate();
        break;
    }
    case WM_DESTROY:
    {
        Window* window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        window->OnDestroy();
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}




bool Window::Init()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.cbClsExtra = NULL;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = NULL;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = NULL;
    wc.lpszClassName = L"WindowClass";

    if (!::RegisterClassEx(&wc))
        return false;
    hWnd = :: CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW, 
        L"WindowClass", 
        L"Pythagoras theorem",
        WS_EX_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        800, 
        600, 
        NULL, 
        NULL, 
        NULL, 
        this);

    if(!hWnd)
    {
        throw std::runtime_error("Sorry, something went up our ass when initialising the window");
    }

    ::ShowWindow(hWnd, SW_SHOW);
    ::UpdateWindow(hWnd);

    isRunning = true;

	return true;
}

bool Window::BroadCast()
{
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    this->OnUpdate();
    Sleep(1);
	return true;
}

bool Window::IsWindowRunning()
{
    return isRunning;
}

bool Window::Realise()
{
    if (::DestroyWindow(hWnd))
    {
        return false;
    }
	return true;
}

void Window::OnCreate()
{

}

void Window::OnDestroy()
{
    isRunning = false;
}

void Window::OnUpdate()
{
}

Window::~Window()
{
}

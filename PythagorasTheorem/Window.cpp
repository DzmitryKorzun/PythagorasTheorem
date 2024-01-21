#include "Window.h"
#include "stdexcept"
#include <d3d12.h>
#include <dxgi1_6.h>

Window::Window()
{
 
}

HWND Window::m_hWnd = nullptr;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>
            (pCreateStruct->lpCreateParams));
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

int Window::RUN(IDX* idxGame, HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = NULL;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = NULL;
    wc.lpszClassName = L"WindowClass";

    RegisterClassEx(&wc);

    RECT windowRECT = { 0, 0, static_cast<LONG>(idxGame->GetWidth()), static_cast<LONG>(idxGame->GetHeight())};

    AdjustWindowRect(&windowRECT, WS_OVERLAPPEDWINDOW, false);

    m_hWnd = CreateWindow(
        wc.lpszClassName,
        idxGame->GetTitle(),
        WS_EX_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRECT.right-windowRECT.left,
        windowRECT.bottom-windowRECT.top,
        nullptr,
        nullptr,
        hInstance,
        idxGame);

    idxGame->OnInit();
    ShowWindow(m_hWnd, nCmdShow);

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    idxGame->OnDestroy();

    return static_cast<char>(msg.wParam);
}

bool Window::Init()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = NULL;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = NULL;
    wc.lpszClassName = L"WindowClass";

    if (!::RegisterClassEx(&wc))
        return false;
    m_hWnd = :: CreateWindowEx(
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

    if(!m_hWnd)
    {
        throw std::runtime_error("Sorry, something went up our ass when initialising the window");
    }

    ::ShowWindow(m_hWnd, SW_SHOW);
    ::UpdateWindow(m_hWnd);

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
    if (::DestroyWindow(m_hWnd))
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

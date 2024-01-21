#include "Game.h"
#include "d3d12.h"
#include <wrl.h>

Game::Game(UINT width, UINT height, std::wstring name) : IDX (width, height, name)
{

}

void Game::OnInit()
{
	LoadPipeLine();
	LoadAssets();
}

void Game::OnUpdate()
{

}

void Game::OnRender()
{

}

void Game::OnDestroy()
{

}

void Game::LoadPipeLine()
{
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif
	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
	if (m_useWarpDevice)
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
		factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
		D3D12CreateDevice(
			warpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		);
	}
	else
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);
		D3D12CreateDevice(
			hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		);
	}

}

void Game::LoadAssets()
{

}

void Game::PopulateCommandList()
{

}

void Game::WaitForPreviousFrame()
{

}

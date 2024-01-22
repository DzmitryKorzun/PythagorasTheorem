#include "Game.h"
#include "d3d12.h"
#include "d3dx12.h"
#include <wrl.h>
#include "Window.h"
#include "d3dcompiler.h"

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

	D3D12_COMMAND_QUEUE_DESC queueDescriptor = {};
	queueDescriptor.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDescriptor.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	m_device->CreateCommandQueue(&queueDescriptor, IID_PPV_ARGS(&m_CommandQueue));


	DXGI_SWAP_CHAIN_DESC1 swapChainDescriptor = {};
	swapChainDescriptor.BufferCount = FrameCount;
	swapChainDescriptor.Width = m_width;
	swapChainDescriptor.Height = m_height;
	swapChainDescriptor.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDescriptor.SampleDesc.Count = 1;
	
	Microsoft::WRL::ComPtr <IDXGISwapChain1> swapChain;
	factory->CreateSwapChainForHwnd(
		m_CommandQueue.Get(),
		Window::GetHwnd(),
		&swapChainDescriptor,
		nullptr,
		nullptr,
		&swapChain
	);

	factory->MakeWindowAssociation(Window::GetHwnd(), 
		DXGI_MWA_NO_ALT_ENTER);

	swapChain.As(&m_swapChain);
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDescriptor = {};
		rtvHeapDescriptor.NumDescriptors = FrameCount;
		rtvHeapDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_device->CreateDescriptorHeap(
			&rtvHeapDescriptor,
			IID_PPV_ARGS(&m_rtvHeap));

		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		for (UINT frame = 0; frame < FrameCount; frame++)
		{
			m_swapChain->GetBuffer(frame, IID_PPV_ARGS(&m_renderTargets[frame]));
			m_device->CreateRenderTargetView(m_renderTargets[frame].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}

	}

	m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllLocator));


}

void Game::LoadAssets()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescriptor;
	rootSignatureDescriptor.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;

	D3D12SerializeRootSignature(&rootSignatureDescriptor, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));

	Microsoft::WRL::ComPtr <ID3DBlob> vertexShader;
	Microsoft::WRL::ComPtr <ID3DBlob> pixelShader;


#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	compileFlags = 0;
#endif
	D3DCompileFromFile(GetAssetsFullPath(L"shader.hlsl").c_str(), nullptr, nullptr, 
		"VsMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);


}

void Game::PopulateCommandList()
{

}

void Game::WaitForPreviousFrame()
{

}

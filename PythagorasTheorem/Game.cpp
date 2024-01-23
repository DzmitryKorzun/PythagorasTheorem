#include "Game.h"
#include "d3d12.h"
#include "d3dx12.h"
#include <wrl.h>
#include "Window.h"
#include "d3dcompiler.h"

Game::Game(UINT width, UINT height, std::wstring name)
	:IDX(width, height, name),
	m_frameIndex(0),
	m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
	m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
	m_rtvDescriptorSize(0)
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
	PopulateCommandList();
	ID3D12CommandList* ppCommandList[] = { m_commandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	m_swapChain->Present(0, 1);
	WaitForPreviousFrame();
}

void Game::OnDestroy()
{
	WaitForPreviousFrame();
	CloseHandle(m_fenceEvent);
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
	D3DCompileFromFile(GetAssetsFullPath(L"Shader.hlsl").c_str(), nullptr, nullptr, 
		"VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);	
	D3DCompileFromFile(GetAssetsFullPath(L"Shader.hlsl").c_str(), nullptr, nullptr, 
		"PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);

	D3D12_INPUT_ELEMENT_DESC inputElementDescriptors[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDescriptor = {};
	psoDescriptor.InputLayout =
	{
		inputElementDescriptors,
		_countof(inputElementDescriptors)
	};
	psoDescriptor.pRootSignature = m_rootSignature.Get();
	psoDescriptor.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDescriptor.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psoDescriptor.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDescriptor.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDescriptor.DepthStencilState.DepthEnable = FALSE;
	psoDescriptor.DepthStencilState.StencilEnable = FALSE;
	psoDescriptor.SampleMask = UINT_MAX;
	psoDescriptor.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDescriptor.NumRenderTargets = 1;
	psoDescriptor.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	psoDescriptor.SampleDesc.Count = 1;
	m_device->CreateGraphicsPipelineState(&psoDescriptor, IID_PPV_ARGS
	(&m_pipelineState));

	m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_CommandAllLocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList));

	m_commandList->Close();

	Vertex triangleVertex[] =
	{
		{{0.0f,0.25f * m_aspectRatio,0.0f},{1.0f,0.0f,0.0f,1.0f}},
		{{0.25f,-0.25f * m_aspectRatio,0.0f},{0.0f,1.0f,0.0f,1.0f}},
		{{0.0f,0.25f * m_aspectRatio,0.0f},{1.0f,0.0f,1.0f,1.0f}},
	};

	const UINT vertexBufferSize = sizeof(triangleVertex);
	CD3DX12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
	m_device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDescriptor,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_vertexBuffer));

	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);
	m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>
		(&pVertexDataBegin));
	memcpy(pVertexDataBegin, triangleVertex, sizeof(triangleVertex));
	m_vertexBuffer->Unmap(0, nullptr);

	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = sizeof(Vertex);
	m_vertexBufferView.SizeInBytes = vertexBufferSize;

	m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	m_fenceValue = 1;
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr)
	{
		std::throw_with_nested(HRESULT_FROM_WIN32(GetLastError()));
	}
		 
	WaitForPreviousFrame();

}

void Game::PopulateCommandList()
{
	CD3DX12_RESOURCE_BARRIER transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_CommandAllLocator->Reset();
	m_commandList->Reset(m_CommandAllLocator.Get(), m_pipelineState.Get());
	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);
	m_commandList->ResourceBarrier(1, &transitionBarrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);

	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float bgClearColor[] = { 1.0f,1.0f,1.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, bgClearColor, 0, nullptr);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_commandList->DrawInstanced(3, 1, 0, 0);

	transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	m_commandList->ResourceBarrier(1, &transitionBarrier);
	m_commandList->Close();
}

void Game::WaitForPreviousFrame()
{
	const UINT64 fence = m_fenceValue;
	m_CommandQueue->Signal(m_fence.Get(), fence);
	m_fenceValue++;

	if (m_fence->GetCompletedValue() < fence)
	{
		m_fence->SetEventOnCompletion(fence, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}			
}

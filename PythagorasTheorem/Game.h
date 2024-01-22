#pragma once
#include "IDX.h"
#include "d3d12.h"
#include "wrl.h"

class Game : public IDX
{
public:
	Game(UINT width, UINT height, std::wstring name);

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();

private:

	static const UINT FrameCount = 2;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllLocator;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

	UINT m_rtvDescriptorSize;

	UINT m_frameIndex;

	void LoadPipeLine();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();
};


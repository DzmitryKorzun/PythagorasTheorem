#include "IDX.h"
#include "wrl.h"
#include "d3d12.h"
#include <iostream>
#include <filesystem>
#include <string>

IDX::IDX(UINT width, UINT height, std::wstring name)

	: m_width(width)
	,m_height(height)
	,m_title(name)
	,m_useWarpDevice(false)
{
	WCHAR assetPath[512];
	GetAssetsPath(assetPath, _countof(assetPath));
	m_assetsPath = assetPath;

	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

}

IDX::~IDX()
{
}

_Use_decl_annotations_
std::wstring IDX::GetAssetsFullPath(LPCWSTR assetsName)
{
	return m_assetsPath + assetsName;
}

void IDX::GetAssetsPath(WCHAR* assetPath, size_t bufferSize)
{
	std::filesystem::path executablePath = std::filesystem::absolute(std::filesystem::path(__FILE__)).parent_path();
	std::filesystem::path projectFolderPath = executablePath / L"PythagorasTheorem";
	std::filesystem::path assetsFolderPath = projectFolderPath / L"Assets";
	std::wstring assetsStringPath = assetsFolderPath.wstring();
	if (assetsStringPath.size() < bufferSize) {
		wcsncpy_s(assetPath, bufferSize, assetsStringPath.c_str(), _TRUNCATE);
	}
	else {
		std::wcerr << L"Error: Buffer size is too small." << std::endl;
	}
}

void IDX::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHightPerfomanceAdapter)
{
	*ppAdapter = nullptr;	
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
	{
		for (UINT adapterIndex = 0;
			SUCCEEDED(factory6->EnumAdapterByGpuPreference(
				adapterIndex,
				requestHightPerfomanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE :
					DXGI_GPU_PREFERENCE_UNSPECIFIED, 
				IID_PPV_ARGS(&adapter))); 
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 descriptor;
			adapter->GetDesc1(&descriptor);
			if (descriptor.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof
			(ID3D12Device), nullptr))) break;
		}
	}

	if (adapter.Get() == nullptr)
	{
		for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter));
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 descriptor;
			adapter->GetDesc1(&descriptor);
			if (descriptor.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof
			(ID3D12Device), nullptr))) break;
		}
		
	}
	*ppAdapter = adapter.Detach();
}

#pragma once
#include "Windows.h"
#include "string"
#include "dxgi1_6.h"
class IDX
{
public:
	IDX(UINT width, UINT height, std::wstring name);
	virtual ~IDX();

	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy() = 0;

	UINT GetWidth() const { return m_width; }
	UINT GetHeight() const { return m_height; }
	const WCHAR* GetTitle() const { return m_title.c_str(); }

protected:

	void GetHardwareAdapter(_In_  IDXGIFactory1* pFactory, _Outptr_opt_result_maybenull_ IDXGIAdapter1** ppAdapter, 
		bool requestHightPerfomanceAdapter = false);

	UINT m_width;
	UINT m_height;

	float m_aspectRatio;

	bool m_useWarpDevice;

	

private:

	std::wstring m_title;

};
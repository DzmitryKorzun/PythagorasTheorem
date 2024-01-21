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

	Microsoft::WRL::ComPtr<ID3D12Device> m_device;

	void LoadPipeLine();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();
};


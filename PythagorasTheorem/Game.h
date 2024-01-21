#pragma once
#include "IDX.h"

class Game : public IDX
{
public:
	Game(UINT width, UINT height, std::wstring name);

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();


};


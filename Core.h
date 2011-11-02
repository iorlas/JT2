#pragma once

#include "DX9Manager.h"
#include "IRenderable.h"

namespace JungleTime{

class Core : public DX9Manager{
public:
	Core(void);
	~Core(void);
	
	int Init(void);	

	static Core* singleton;
private:
	void OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice);
	void OnDXInitiated(void);
	void OnDXEndScene(LPDIRECT3DDEVICE9 pDevice);
	void OnDXResetDevice(LPDIRECT3DDEVICE9 pDevice);
	void OnDXLostDevice();

	void InitTimePointers();

	vector<IRenderableObject*> renderObjects;

	//Pointers to in-game clock values
	float* ingameClockNow;
	float* ingameClockStart;

	//For some expensive operations, we cant do things every frame, so we need a counter to
	// make operations do things "each N frames"
	int framesCounter;
};

}

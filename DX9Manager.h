#pragma once

#include "defines.h"

namespace JungleTime{

class DX9Manager
{
public:
	DX9Manager(void);
	~DX9Manager(void);

	static unsigned int WINAPI MainHookDX9(void *param);

	//Our dirty hooks...
	static void HookEndScene(LPDIRECT3DDEVICE9 pDevice);
	static void HookPreReset();
	static void HookPostReset(LPDIRECT3DDEVICE9 pDevice, HRESULT res);

	//Temporary indicators for the EndScene
	bool isInitiated;
	bool isFirstFrame;
	bool isFirstInitiatedFrame;

	static DX9Manager* singleton;
private:

	//On the first frame we intercept
	virtual void OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice);
	//On hooking done
	virtual void OnDXInitiated(void);
	//On each frame
	virtual void OnDXEndScene(LPDIRECT3DDEVICE9 pDevice);
	//On reset
	virtual void OnDXResetDevice(LPDIRECT3DDEVICE9 pDevice);
	//On resources unloading
	virtual void OnDXLostDevice();

	HANDLE hDXHookThread;

	//For the "loading..." label
	LPD3DXFONT consoleFont;
	RECT consoleRect;

	LPDIRECT3DDEVICE9 d3d9Device;
	bool isDeviceReady;
};

}
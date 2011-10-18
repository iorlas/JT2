#pragma once

#include "defines.h"

namespace JungleTime{

class DX9Manager
{
public:
	DX9Manager(void);
	~DX9Manager(void);

	void MainHookDX9(void);

	//Our dirty hooks...
	static void HookEndScene(LPDIRECT3DDEVICE9 pDevice);

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

	boost::shared_ptr<boost::thread> thrDXHook;

	//For the "loading..." label
	LPD3DXFONT consoleFont;
	RECT consoleRect;

	LPDIRECT3DDEVICE9 d3d9Device;
};

}
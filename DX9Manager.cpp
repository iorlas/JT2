#include "DX9Manager.h"

namespace JungleTime{

DX9Manager* DX9Manager::singleton = NULL;

//Original funcs to call after hook
DX9Manager::DX9Manager(void):
	isInitiated(false), isFirstFrame(false), isFirstInitiatedFrame(false),
	consoleFont(0){
	LOG_DEBUG_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"Init", L"starting...");
	if(singleton)
		LOG_ERROR_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"Init", L"dude... you are idiot, we cant create more singletons >:(");
	singleton = this;
	SetRect(&consoleRect, 20, 20, 100, 100);
	hDXHookThread = CreateThread(0, 1024, DX9Manager::MainHookDX9, 0, 0, 0);
	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"Init", L"done");
}

DX9Manager::~DX9Manager(void){
	LOG_DEBUG_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"Close", L"unloading...");

	//Unhook functions
	typedef UINT (WINAPI* D3D9DeviceFuncUnHookFunc)(UINT funcId);
	D3D9DeviceFuncUnHookFunc D3D9DeviceFuncUnHook = (D3D9DeviceFuncUnHookFunc)GetProcAddress(GetModuleHandle(L"d3d9.dll"), "D3D9DeviceFuncUnHook");
	if(!D3D9DeviceFuncUnHook){
		LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"Close", L"cannot find D3D9DeviceFuncUnHook function in the d3d9.dll! Maybe broken D3D9 Proxy?");
		MessageBox(NULL, L"JT2 Lib Error", L"Cannot load special functions. You need to use special d3d9.dll proxy library!", MB_OK | MB_ICONERROR);
		return;
	}
	D3D9DeviceFuncUnHook(ENDSCENE);

	//Release resources only if application loop is alive, we dont need to freeze application in the process list
	if(!IS_DX_LOOP_DEAD)
		singleton->consoleFont->Release();

	CloseHandle(hDXHookThread);

	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"Close", L"unloaded");
}

void DX9Manager::HookEndScene(LPDIRECT3DDEVICE9 pDevice){
	//At first frame, we need to init every dx thing
	if(!singleton->isFirstFrame){
		LOG_DEBUG_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"OnFirstFrame", L"first frame catched");
		singleton->d3d9Device = pDevice;
		D3DXCreateFont(pDevice, 15, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Verdana", &singleton->consoleFont);
		singleton->isFirstFrame = true;
	}
	
	//For the loading time
	if(!singleton->isInitiated)
		singleton->consoleFont->DrawText(NULL, L"JT2 init...", -1, &singleton->consoleRect, DT_CENTER|DT_NOCLIP, 0xFFFFFFFF);
	else{
		//First frame after initiation of the real application is complete
		if(!singleton->isFirstInitiatedFrame){
			LOG_DEBUG_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"OnFirstFrame", L"first initiated app frame catched");
			singleton->OnDXFirstFrame(pDevice);
			singleton->isFirstInitiatedFrame = true;
		}
		//Touch event...
		singleton->OnDXEndScene(pDevice);
	}
}

DWORD WINAPI DX9Manager::MainHookDX9(LPVOID Param){
	//Waiting for d3d lib is loaded
	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"MainThread", L"waiting for the lib");
    while(GetModuleHandle(L"d3d9.dll")==NULL)
		Sleep(250);
	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"MainThread", L"d3d9.dll detected");

	//Load library(assuming it's our proxy d3d9 lib) and functions to hook end scene.
	typedef UINT (WINAPI* D3D9DeviceFuncHookFunc)(UINT funcId, void* funcRef);
	D3D9DeviceFuncHookFunc D3D9DeviceFuncHook = (D3D9DeviceFuncHookFunc)GetProcAddress(GetModuleHandle(L"d3d9.dll"), "D3D9DeviceFuncHook");
	if(!D3D9DeviceFuncHook){
		LOG_ERROR_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"MainThread", L"cannot find D3D9DeviceFuncHook function in the d3d9.dll! Maybe broken D3D9 Proxy?");
		MessageBox(NULL, L"JT2 Lib Error", L"Cannot load special functions. You need to use special d3d9.dll proxy library!", MB_OK | MB_ICONERROR);
		return 0;
	}

	//Trying to hook
	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"MainThread", L"waiting for the dx proxy load...");
	UINT res = NULL;
	do{
		Sleep(250);
		res = D3D9DeviceFuncHook(ENDSCENE, &HookEndScene);
	}while (!res);
	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"MainThread", L"hooking done");
	
	//Touch our event...
	singleton->OnDXInitiated();

	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"MainThread", L"thread work done");
	return 0;
}

void DX9Manager::OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice){
	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"OnDXFirstFrame", L"On First Frame event unhooked!");
}
void DX9Manager::OnDXInitiated(void){
	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"OnDXInitiated", L"On Init event unhooked!");
}
void DX9Manager::OnDXEndScene(LPDIRECT3DDEVICE9 pDevice){
	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"OnDXEndScene", L"On End Scene event unhooked!");
}

}
#include "DX9Manager.h"

namespace JungleTime{

DX9Manager* DX9Manager::singleton = NULL;

//Original funcs to call after hook
DX9Manager::DX9Manager(void):
	isInitiated(false), isFirstFrame(false),
	consoleFont(0){
	if(singleton)
		LOG_VERBOSE(L"DX9 manager: dude... you are idiot, we cant create more singletons >:(");
	singleton = this;
	SetRect(&consoleRect, 20, 20, 100, 100);
	thrDXHook = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&DX9Manager::MainHookDX9, this)));
	LOG_VERBOSE(L"* DX9 manager initiated");
}

DX9Manager::~DX9Manager(void){
	//Unhook functions
	typedef UINT (WINAPI* D3D9DeviceFuncUnHookFunc)(UINT funcId);
	D3D9DeviceFuncUnHookFunc D3D9DeviceFuncUnHook = (D3D9DeviceFuncUnHookFunc)GetProcAddress(GetModuleHandle(L"d3d9.dll"), "D3D9DeviceFuncUnHook");
	if(!D3D9DeviceFuncUnHook){
		MessageBox(NULL, L"JT2 Lib Error", L"Cannot load special functions. You need to use special d3d9.dll proxy library!", MB_OK | MB_ICONERROR);
		return;
	}
	D3D9DeviceFuncUnHook(ENDSCENE);

	//Release resources
	singleton->consoleFont->Release();

	LOG_VERBOSE(L"* DX9 manager closed");
}

void DX9Manager::HookEndScene(LPDIRECT3DDEVICE9 pDevice){
	//At first frame, we need to init every dx thing
	if(!singleton->isFirstFrame){
		D3DXCreateFont(pDevice, 15, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Verdana", &singleton->consoleFont);
		singleton->OnDXFirstFrame(pDevice);
		singleton->isFirstFrame = true;
	}
	
	//For the loading time
	if(!singleton->isInitiated)
		singleton->consoleFont->DrawText(NULL, L"JT2 init...", -1, &singleton->consoleRect, DT_CENTER|DT_NOCLIP, 0xFFFFFFFF);

	//Touch event...
	singleton->OnDXEndScene(pDevice);	
}

void DX9Manager::MainHookDX9(void){
	//Waiting for d3d lib is loaded
	LOG_VERBOSE(L"DX9 manager: waiting for the lib");
    while(GetModuleHandle(L"d3d9.dll")==NULL)
		Sleep(250);
	LOG_VERBOSE(L"DX9 manager: d3d9.dll detected");

	//Load library(assuming it's our proxy d3d9 lib) and functions to hook end scene.
	typedef UINT (WINAPI* D3D9DeviceFuncHookFunc)(UINT funcId, void* funcRef);
	D3D9DeviceFuncHookFunc D3D9DeviceFuncHook = (D3D9DeviceFuncHookFunc)GetProcAddress(GetModuleHandle(L"d3d9.dll"), "D3D9DeviceFuncHook");
	if(!D3D9DeviceFuncHook){
		MessageBox(NULL, L"JT2 Lib Error", L"Cannot load special functions. You need to use special d3d9.dll proxy library!", MB_OK | MB_ICONERROR);
		return;
	}

	//Trying to hook
	LOG_VERBOSE(L"DX9 manager: waiting for the dx proxy load...");
	UINT res = NULL;
	do{
		Sleep(250);
		res = D3D9DeviceFuncHook(ENDSCENE, &HookEndScene);
	}while (!res);
	LOG_VERBOSE(L"DX9 manager: hooking done");
	
	//Touch our event...
	OnDXInitiated();
}

void DX9Manager::OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice){
	LOG_VERBOSE(L"DX9 manager: On First Frame event unhooked!");
}
void DX9Manager::OnDXInitiated(void){
	LOG_VERBOSE(L"DX9 manager: On Init event unhooked!");
}
void DX9Manager::OnDXEndScene(LPDIRECT3DDEVICE9 pDevice){
	LOG_VERBOSE(L"DX9 manager: On End Scene event unhooked!");
}

}
#include "DX9Manager.h"

namespace JungleTime{

DX9Manager* DX9Manager::singleton = NULL;

//Original funcs to call after hook
DX9Manager::DX9Manager(void):
	isInitiated(false), isFirstFrame(false), isFirstInitiatedFrame(false),
	statusFont(0), isDeviceReady(true), statusText(L"JT2 init..."), isRenderStatus(true){
	LOG_DEBUG_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"Init", L"starting...");
	if(singleton)
		LOG_ERROR_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"Init", L"dude... you are idiot, we cant create more singletons >:(");
	singleton = this;
	SetRect(&statusRect, 20, 20, 100, 100);
	hDXHookThread = (HANDLE)_beginthreadex(NULL, 0, DX9Manager::MainHookDX9, NULL, 0, NULL);
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
	D3D9DeviceFuncUnHook(PRERESET);
	D3D9DeviceFuncUnHook(POSTRESET);

	//Release resources only if application loop is alive, we dont need to freeze application in the process list
	if(!IS_DX_LOOP_DEAD)
		singleton->statusFont->Release();

	CloseHandle(hDXHookThread);

	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"Close", L"unloaded");
}

void DX9Manager::HookEndScene(LPDIRECT3DDEVICE9 pDevice){
	//I prefer to check it each time
	HRESULT r = pDevice->TestCooperativeLevel();
	if(r == D3DERR_DEVICELOST){
		singleton->isDeviceReady = false;
		//Ok, lost, now we can free our old resources
		LOG_WARNING_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"HookEndScene", L"device is LOST!");
		Sleep(500); //Wait a bit so we don't burn through cycles for no reason (c) http://www.drunkenhyena.com/cgi-bin/view_cpp_article.pl?chapter=2;article=10
		return;
	}else if(r == D3DERR_DEVICENOTRESET){
		singleton->isDeviceReady = false;
		LOG_WARNING_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"HookEndScene", L"device can be resetted, waiting for the main app Device->Reset call");
		return;
	}else if(r != D3D_OK){
		LOG_WARNING_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"HookEndScene", L"unknown state of device, skip renderring");
		Sleep(500); //Wait a bit so we don't burn through cycles for no reason (c) http://www.drunkenhyena.com/cgi-bin/view_cpp_article.pl?chapter=2;article=10
		return;
	}

	//If device wasnt ready for this moment - we have nothing to do
	if(!singleton->isDeviceReady)
		return;

	//At first frame, we need to init every dx thing
	if(!singleton->isFirstFrame){
		LOG_DEBUG_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"OnFirstFrame", L"first frame catched");
		singleton->d3d9Device = pDevice;
		D3DXCreateFont(pDevice, 15, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Verdana", &singleton->statusFont);
		singleton->isFirstFrame = true;
	}

	if(singleton->isRenderStatus)
		singleton->statusFont->DrawText(NULL, singleton->statusText.c_str(), -1, &singleton->statusRect, DT_CENTER|DT_NOCLIP, 0xFFFFFFFF);	

	if(singleton->isInitiated){
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

void DX9Manager::HookPreReset(){
	singleton->statusFont->OnLostDevice();
	singleton->OnDXLostDevice();
	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"HookReset", L"all resources are unloaded");

	singleton->isDeviceReady = false;
}

void DX9Manager::HookPostReset(LPDIRECT3DDEVICE9 pDevice, HRESULT res){
	if(!SUCCEEDED(res)){
		LOG_DEBUG_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"HookPostReset", L"unsuccessfull reset of device");
		return;
	}
	LOG_DEBUG_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"HookPostReset", L"successfull reset of device");
	
	singleton->statusFont->OnResetDevice();
	singleton->OnDXResetDevice(pDevice);
	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"HookPostReset", L"all resources are ready");

	singleton->isDeviceReady = true;
}

unsigned int WINAPI DX9Manager::MainHookDX9(void *param){
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
	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"MainThread", L"waiting for the dx proxy load and hooking things");
	UINT res = NULL;
	do{
		Sleep(250);
		res = D3D9DeviceFuncHook(ENDSCENE, &HookEndScene);
	}while (!res);

	do{
		Sleep(250);
		res = D3D9DeviceFuncHook(PRERESET, &HookPreReset);
	}while (!res);

	do{
		Sleep(250);
		res = D3D9DeviceFuncHook(POSTRESET, &HookPostReset);
	}while (!res);
	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"MainThread", L"hooking is done");
	
	//Touch our event...
	singleton->OnDXInitiated();

	LOG_VERBOSE_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"MainThread", L"thread work is done");

	return 0;
}

void DX9Manager::SetStatusText(wstring txt){
	statusText = txt;
}
void DX9Manager::SetStatusIsRender(bool isRender){
	isRenderStatus = isRender;
}

void DX9Manager::OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice){
	LOG_WARNING_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"OnDXFirstFrame", L"event unhooked!");
}
void DX9Manager::OnDXInitiated(void){
	LOG_WARNING_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"OnDXInitiated", L"event unhooked!");
}
void DX9Manager::OnDXEndScene(LPDIRECT3DDEVICE9 pDevice){
	LOG_WARNING_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"OnDXEndScene", L"event unhooked!");
}
void DX9Manager::OnDXResetDevice(LPDIRECT3DDEVICE9 pDevice){
	LOG_WARNING_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"OnDXResetDevice", L"event unhooked!");
}
void DX9Manager::OnDXLostDevice(){
	LOG_WARNING_MF(L"DX9Manager.cpp", L"DirectX9Manager", L"OnDXLostDevice", L"event unhooked!");
}

}
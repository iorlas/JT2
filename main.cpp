#include "defines.h"
#include "Core.h"

LOG_DECLARE;

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved){
	DisableThreadLibraryCalls(hModule);
    if(dwReason == DLL_PROCESS_ATTACH){
    	int res;
		LOG_INIT("jt2");
		LOG_DEBUG_MF(L"main.cpp", L"DLL", L"Attach", L"in process...");
		JungleTime::Core::singleton = new JungleTime::Core();
		res = JungleTime::Core::singleton->Init();
		LOG_VERBOSE_MF(L"main.cpp", L"DLL", L"Attach", L"complete");
		return res;
    }
    else if(dwReason == DLL_PROCESS_DETACH){
    	LOG_DEBUG_MF(L"main.cpp", L"DLL", L"Detach", L"in progress...");
		delete JungleTime::Core::singleton;
		LOG_VERBOSE_MF(L"main.cpp", L"DLL", L"Detach", L"complete");
    }
    return TRUE;
}
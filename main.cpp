#include "defines.h"
#include "Core.h"

LOG_DECLARE;

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved){
	DisableThreadLibraryCalls(hModule);
    if(dwReason == DLL_PROCESS_ATTACH){
		LOG_INIT("jt2.log");
		LOG_VERBOSE(L"* DLL Attach - in process");
		JungleTime::Core::singleton = new JungleTime::Core();
		LOG_VERBOSE(L"* DLL Attach - complete, init Core");
		return JungleTime::Core::singleton->Init();
    }
    else if(dwReason == DLL_PROCESS_DETACH){
		LOG_VERBOSE(L"* DLL Detach - in process");
		delete JungleTime::Core::singleton;
		LOG_VERBOSE(L"* DLL Detach - done");
    }
    return TRUE;
}
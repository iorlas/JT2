#include "defines.h"


LPCWSTR INIReadStr(LPCWSTR cat, LPCWSTR name, LPCWSTR fileName){
	wchar_t *buff = new wchar_t[255];
	memset(buff, 0x00, 255);
	int res = GetPrivateProfileString(cat, name, L"NONE_STR", buff, 255, fileName);
	if(res == -2 || res == -3){
		LOG_WARNING_MF(L"ObjectTimer.cpp", cat, name, L"INI Reader error: buffer too small");
		delete buff;
		return L"ERR1";
	}
	if(res == -1){
		res = GetLastError();
		if(res == 0x2){
			LOG_ERROR_MF(L"ObjectTimer.cpp", L"INIReader", fileName, L"config file not found");
			delete buff;
			return L"ERR404";
		}
	}
	return buff;
}

int INIReadInt(LPCWSTR cat, LPCWSTR name, LPCWSTR fileName){
	int res = GetPrivateProfileInt(cat, name, -1, fileName);
	if(res == -1 && GetLastError() == 0x2)
		LOG_ERROR_MF(L"ObjectTimer.cpp", L"INIReader", fileName, L"config file not found");
	return res;
}

int TryCopyMem(void *dest, const void *src, size_t count){
	__try{
		return (int)memcpy(dest, src, count);
	}__except(EXCEPTION_EXECUTE_HANDLER){
		return GetExceptionCode();
	}
}
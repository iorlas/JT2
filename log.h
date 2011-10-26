#ifndef _LOG_H
#define _LOG_H
//It is bad to include files within other include files, but this is an exception to simplify the code
#include <stdio.h>
#include <windows.h>
#include <time.h>

//Yep, im noob, ololo
#define WIDEN2(x) L##x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

//must be put in global scope and used once in all files
#define LOG_DECLARE Log __log;
//used in any file need to access the log files, except the file that has LOG_DECLARE statment
#define LOG_USE extern Log __log;
//init log files, called once in program initialization
#define LOG_INIT(fileName) do{if(fileName!= NULL)__log.init(fileName); else printf("error init log file %s", fileName);}while(0);

//here are the different levels of logging
#define LOG_VERBOSE(msg) __log.write(L"verbose", msg, __WFILE__, __LINE__); //detailed info
#define LOG_MSG(msg) __log.write(L"msg", msg, __WFILE__, __LINE__); //brief info
#define LOG_WARN(msg) __log.write(L"warn", msg, __WFILE__, __LINE__); //warning
#define LOG_ERR(msg) __log.write(L"error", msg, __WFILE__, __LINE__); //error
#define LOG_FATAL(msg) __log.write(L"fatal", msg, __WFILE__, __LINE__); //fatal error

class Log
{
	FILE *fp;
	bool logOk;
	HANDLE mutex;
public:
	Log() {
		fp = NULL;
	}
	~Log() {
		close();
	}

	//Hide deprecation warning, it's just a trashy output...
	#pragma warning(push)
	#pragma warning(disable: 4996)
	void init(char *pfileName) {
		if(pfileName != NULL) {
			fp = fopen(pfileName, "a+");
			if(fp != NULL) fseek(fp, 0, SEEK_END);
		}
		mutex = CreateMutex(NULL, FALSE, L"LogFileMutex");
	}
	#pragma warning(pop)

	void close() {
		if(fp != NULL) {
			fwprintf(fp, L"****************End of log****************\n");
			fclose(fp);
		}
		fp = NULL;
		CloseHandle(mutex);
	}

	void write(LPCWSTR pType, LPCWSTR pMsg, LPCWSTR pFileName, int lineNo) {
		DWORD dwWaitResult = 0;
		while(dwWaitResult != WAIT_OBJECT_0) {
			dwWaitResult = WaitForSingleObject(mutex, 1);
		}
		if(fp != NULL) {
			fwprintf(fp, L"%s\t%lld\t%s\t%d\t%s\n", pType, time(NULL), pFileName, lineNo, pMsg);
			fflush(fp);
		}
		ReleaseMutex(mutex);
	}

	void write_ptr(LPCWSTR pType, LPCWSTR pMsg, LPCWSTR pFileName, int lineNo, DWORD ptr) {
		DWORD dwWaitResult = 0;
		while(dwWaitResult != WAIT_OBJECT_0) {
			dwWaitResult = WaitForSingleObject(mutex, 1);
		}
		if(fp != NULL) {
			fwprintf(fp, L"%s\t%lld\t%s\t%d\t%s - %p\n", pType, time(NULL), pFileName, lineNo, pMsg, ptr);
			fflush(fp);
		}
		ReleaseMutex(mutex);
	}
};

#endif
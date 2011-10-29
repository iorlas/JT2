#ifndef _LOG_H
#define _LOG_H
//It is bad to include files within other include files, but this is an exception to simplify the code
#include <stdio.h>
#include <windows.h>
#include <time.h>

//Yep, im noob, ololo, but this is a one way i know how to convert __FILE__ to wchar string
#define WIDEN2(x) L##x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#define __WFUNCTION__ WIDEN(__FUNCTION__)

//put it anywhere, better right before LOG_INIT
#define LOG_DECLARE Log __log;
//use in global header or in any needed part of code
#define LOG_USE extern Log __log;
//init log files, called once in program initialization
#define LOG_INIT(fileName) __log.Init(fileName);

/* Warning!!! __FILE__ can be file name with a full path to file.
 * Yeah, i know about special flag, but in headers it CANNOT be just a file name. "By design" (c) MS
 * So we need to enter it manually. MF stands for Manual __FILE__.
 */

//EF stands for Each Frame. This mode needs for debugging of the some things, like render cycle.
//#define JT_EFDEBUG
#define JT_DEBUG

#define _LOG_MSG_MF(fileName, pType, objStr, childObjStr, msg) __log.WriteStr(##pType, __WFUNCTION__, ##fileName, __LINE__, ##objStr, ##childObjStr, ##msg)
#if defined(_DEBUG) || defined(JT_DEBUG)
#define LOG_DEBUG_MF(fileName, objStr, childObjStr, msg) _LOG_MSG_MF(##fileName, L"DEBUG", ##objStr, ##childObjStr, ##msg) //add to code only in debug version. DEEP debug messages. A lot of 'em.
#else
#define LOG_DEBUG_MF(fileName, objStr, childObjStr, msg) ;
#endif

#if (defined(_DEBUG) || defined(JT_DEBUG)) && defined(JT_EFDEBUG)
#define LOG_DEBUG_EF_MF(fileName, objStr, childObjStr, msg) _LOG_MSG_MF(##fileName, L"EFDEB", ##objStr, ##childObjStr, ##msg) //add to code only in debug version. DEEP debug messages. A lot of 'em.
#else
#define LOG_DEBUG_EF_MF(fileName, objStr, childObjStr, msg) ;
#endif

#define LOG_VERBOSE_MF(fileName, objStr, childObjStr, msg) _LOG_MSG_MF(##fileName, L"VERBO", ##objStr, ##childObjStr, ##msg) //common messages for a general things.
#define LOG_WARNING_MF(fileName, objStr, childObjStr, msg) _LOG_MSG_MF(##fileName, L"WARNI", ##objStr, ##childObjStr, ##msg) //error in anything. But we still can display something.
#define LOG_ERROR_MF(fileName, objStr, childObjStr, msg) _LOG_MSG_MF(##fileName, L"ERROR", ##objStr, ##childObjStr, ##msg) //real errors, we cannot continue use Jungle Timer.

class Log{
private:
	FILE *fp;
	bool logOk;
	HANDLE mutex;
public:
	Log(){
		fp = NULL;
	}

	~Log(){
		Close();
	}

	//Hide deprecation warning, it's just a trashy output...
	#pragma warning(push)
	#pragma warning(disable: 4996)
	void Init(char *pfileName){
		if(pfileName != NULL){
			//Add date to a file name
			SYSTEMTIME lt;
			GetLocalTime(&lt);
			char buffLogName[50];
	  		sprintf(buffLogName, "%s_%02dm%02dd%02dy%s", pfileName, lt.wMonth, lt.wDay, lt.wYear, ".log");

	  		//Finally, open the file
			fp = fopen(buffLogName, "a+");
			if(fp != NULL) fseek(fp, 0, SEEK_END);
		}
		mutex = CreateMutex(NULL, FALSE, L"LogFileMutex");
	}
	#pragma warning(pop)

	void Close(){
		if(fp != NULL){
			fwprintf(fp, L"*****************************End of log*****************************\n");
			fclose(fp);
		}
		fp = NULL;
		CloseHandle(mutex);
	}

	void WriteStr(LPCWSTR pType, LPCWSTR pFuncName, LPCWSTR pFileName, int lineNo, LPCWSTR objStr, LPCWSTR childObjStr, LPCWSTR msg){
		if(fp == NULL)
			return;
		
		SYSTEMTIME lt;
		GetLocalTime(&lt);

		DWORD dwWaitResult = 0;
		while(dwWaitResult != WAIT_OBJECT_0){
			dwWaitResult = WaitForSingleObject(mutex, 1);
		}

		fwprintf(fp, L"%s <%02d:%02d:%02d.%03d> %s at [%s:%d]\t%s.%s >> %s\n", pType, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds, pFuncName, pFileName, lineNo, objStr, childObjStr, msg);
		fflush(fp);

		ReleaseMutex(mutex);
	}
};

#endif
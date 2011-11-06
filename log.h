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

//OD stands for OutputDebug. Special debug output for MSVS. Can be handled using MSVS Output window or DebugView
//#define JT_ODDEBUG
//EF stands for Each Frame. This mode needs for debugging of the some things, like render cycle. WARN! You need to use Debug with this flag
//#define JT_EFDEBUG
//Just debug...
//#define JT_DEBUG

#define _LOG_MSG_MF(fileName, pType, gametime, objStr, childObjStr, msg) __log.WriteStr(##pType, __WFUNCTION__, ##fileName, __LINE__, ##gametime, ##objStr, ##childObjStr, ##msg)
#define _LOG_MSG_PTR_MF(fileName, pType, gametime, objStr, childObjStr, msg, ptr) __log.WritePtr(##pType, __WFUNCTION__, ##fileName, __LINE__, ##gametime, ##objStr, ##childObjStr, ##msg, ##ptr)

#if defined(_DEBUG) || defined(JT_DEBUG)
#define LOG_DEBUG_MF(fileName, gametime, objStr, childObjStr, msg) _LOG_MSG_MF(##fileName, L"DEBUG", ##gametime, ##objStr, ##childObjStr, ##msg) //add to code only in debug version. DEEP debug messages. A lot of 'em.
#else
#define LOG_DEBUG_MF(fileName, gametime, objStr, childObjStr, msg) ;
#endif

#if (defined(_DEBUG) || defined(JT_DEBUG)) && defined(JT_EFDEBUG)
#define LOG_DEBUG_EF_MF(fileName, gametime, objStr, childObjStr, msg) _LOG_MSG_MF(##fileName, L"EFDEB", ##gametime, ##objStr, ##childObjStr, ##msg) //add to code only in debug version. DEEP debug messages. A lot of 'em.
#else
#define LOG_DEBUG_EF_MF(fileName, gametime, objStr, childObjStr, msg) ;
#endif

#define LOG_VERBOSE_MF(fileName, gametime, objStr, childObjStr, msg) _LOG_MSG_MF(##fileName, L"VERBO", ##gametime, ##objStr, ##childObjStr, ##msg) //common messages for a general things.
#define LOG_WARNING_MF(fileName, gametime, objStr, childObjStr, msg) _LOG_MSG_MF(##fileName, L"WARNI", ##gametime, ##objStr, ##childObjStr, ##msg) //error in anything. But we still can display something.
#define LOG_ERROR_MF(fileName, gametime, objStr, childObjStr, msg) _LOG_MSG_MF(##fileName, L"ERROR", ##gametime, ##objStr, ##childObjStr, ##msg) //real errors, we cannot continue use Jungle Timer.
#define LOG_ERROR_CODE_MF(fileName, gametime, objStr, childObjStr, msg, code) _LOG_MSG_PTR_MF(##fileName, L"ERROR", ##gametime, ##objStr, ##childObjStr, ##msg, ##code) //real errors with err code, we cannot continue use Jungle Timer.

#define LOG_VERBOSE_PTR_MF(fileName, gametime, objStr, childObjStr, msg, ptr) _LOG_MSG_PTR_MF(##fileName, L"VERBO", ##gametime, ##objStr, ##childObjStr, ##msg, ##ptr)

class Log{
private:
	FILE *fp;
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
			if(fp != NULL){
				fseek(fp, 0, SEEK_END);
				fwprintf(fp, L"\n***************************** NEW LOG START *****************************\n");
			}
		}
		mutex = CreateMutex(NULL, FALSE, L"JT211LogFileMutex");
	}
	#pragma warning(pop)

	void Close(){
		if(fp != NULL)
			fclose(fp);
		fp = NULL;
		CloseHandle(mutex);
	}

	void WriteStr(LPCWSTR pType, LPCWSTR pFuncName, LPCWSTR pFileName, int lineNo, int gameTimeSec, LPCWSTR objStr, LPCWSTR childObjStr, LPCWSTR msg){
		#ifdef JT_ODDEBUG
		wchar_t buf[2048];
		swprintf(buf, 2048, L"(%05d) %s %s at [%s:%d]\t%s.%s > %s\n", gameTimeSec, pType, pFuncName, pFileName, lineNo, objStr, childObjStr, msg);
		OutputDebugString(buf);
		#endif

		if(fp == NULL)
			return;

		SYSTEMTIME lt;
		GetLocalTime(&lt);

		//TODO: Handle errors
		DWORD dwWaitResult = WaitForSingleObject(mutex, INFINITE);
		switch(dwWaitResult){
		case WAIT_ABANDONED:
			break;
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			break;
		case WAIT_FAILED:
			break;
		}

		fwprintf(fp, L"%s <%02d:%02d:%02d.%03d(%05d)> %s at [%s:%d]\t%s.%s >> %s\n", pType, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds, gameTimeSec, pFuncName, pFileName, lineNo, objStr, childObjStr, msg);
		fflush(fp);

		ReleaseMutex(mutex);
	}

	void WritePtr(LPCWSTR pType, LPCWSTR pFuncName, LPCWSTR pFileName, int lineNo, int gameTimeSec, LPCWSTR objStr, LPCWSTR childObjStr, LPCWSTR msg, DWORD ptr){
		#ifdef JT_ODDEBUG
		wchar_t buf[2048];
		swprintf(buf, 2048, L"(%05d) %s %s at [%s:%d]\t%s.%s > %s %p\n", gameTimeSec, pType, pFuncName, pFileName, lineNo, objStr, childObjStr, msg, ptr);
		OutputDebugString(buf);
		#endif

		if(fp == NULL)
			return;

		SYSTEMTIME lt;
		GetLocalTime(&lt);

		//TODO: Handle errors
		DWORD dwWaitResult = WaitForSingleObject(mutex, INFINITE);
		switch(dwWaitResult){
		case WAIT_ABANDONED:
			break;
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			break;
		case WAIT_FAILED:
			break;
		}

		fwprintf(fp, L"%s <%02d:%02d:%02d.%03d(%05d)> %s at [%s:%d]\t%s.%s >> %s %p\n", pType, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds, gameTimeSec, pFuncName, pFileName, lineNo, objStr, childObjStr, msg, ptr);
		fflush(fp);

		ReleaseMutex(mutex);
	}
};

#endif
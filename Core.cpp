#include "defines.h"
#include "Core.h"

#include "ObjectTimer.h"
#include "SmiteIgniteIndicator.h"

namespace JungleTime{

Core* Core::singleton;

Core::Core(void) : DX9Manager(),
	ingameClockNow(NULL), ingameClockStart(NULL){
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Init", L"done");
}

Core::~Core(void){
	LOG_DEBUG_MF(L"Core.cpp", L"Core", L"Deinit", L"unloading...");

	//Kill it with holy fire!
	for (vector<IRenderableObject*>::iterator it = renderObjects.begin(); it != renderObjects.end(); it++)
		delete *it;

	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Deinit", L"unloaded");
}

int Core::Init(void){
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Init2", L"done");
	return TRUE;
}

#define TIMER_NEW(innname, cd, spawnat, mempattern) renderObjects.push_back(new ObjectTimer(##innname, ##cd, ##spawnat, ##mempattern));
#define INDICATOR_NEW(innname, flat, perlvl) renderObjects.push_back(new SmiteIgniteIndicator(##innname, ##flat, ##perlvl));
bool Core::OnDXInitiated(void){
	//Check current map for support of our timers
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Init3", L"waiting for the map");
	int tmp = 0;

	SetStatusText(L"JT2: Loading the map");

	//Load map name
	char buff[10];
	int errCode;
	do{
		//If string is zero-size, it means map wasnt loaded. Yet. We need to wait some time.
		Sleep(10);
		if((errCode = TryCopyMem(buff, (void*)LOL_MEM_MAP_NAME_OFFSET, 5)) != (int)buff){
			LOG_ERROR_CODE_MF(L"Core.cpp", L"Core", L"Init3", L"cannot read map name memory, error code: ", errCode);
			SetStatusText(L"JT2: Cannot read map name memory, see log file.");
			return false;
		}
	}while((tmp = strlen(buff)) == 0);

	LOG_DEBUG_MF(L"Core.cpp", L"Core", L"Objects", L"map found");

	//Compare string, just like a usual string, it's just a bytes
	//Summoners Rift and its versions
	if((strcmp(LOL_MAP_SUMMONERS_RIFT_NAME, (char*)LOL_MEM_MAP_NAME_OFFSET) == 0) ||
		(strcmp(LOL_MAP_SUMMONERS_RIFT_AUT_NAME, (char*)LOL_MEM_MAP_NAME_OFFSET) == 0)){
		tmp = LOL_MAP_SUMMONERS_RIFT;
		LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Objects", L"current map is Summoners Rift");
		SetStatusText(L"JT2: Summoners Rift is loaded");
	//Unknown
	}else{
		tmp = 0;
		LOG_ERROR_MF(L"Core.cpp", L"Core", L"Objects", L"unknown map loaded!");
		SetStatusText(L"JT2: Cannot determine name of the map, see log file.");
		return false;
	}

	SetStatusText(L"JT2: Loading timers");
	LOG_DEBUG_MF(L"Core.cpp", L"Core", L"Objects", L"creating...");
	//We support only Summoners Rift map for jungle camps
	if(tmp == LOL_MAP_SUMMONERS_RIFT){
		//Loading all the timers
		LOG_DEBUG_MF(L"Core.cpp", L"Core", L"Timers", L"creating...");
		// TIMER_NEW(L"baron", 420, 900, LOL_MEM_NETOBJECT_BARON_PATTERN);
		// TIMER_NEW(L"dragon", 360, 150, LOL_MEM_NETOBJECT_DRAGON_PATTERN);

		// TIMER_NEW(L"top_blue", 300, 115, LOL_MEM_NETOBJECT_TOP_BLUE_PATTERN);
		// TIMER_NEW(L"top_red", 300, 115, LOL_MEM_NETOBJECT_TOP_RED_PATTERN);
		// TIMER_NEW(L"top_wraiths", 100, 100, LOL_MEM_NETOBJECT_TOP_WRAITHS_PATTERN);
		// TIMER_NEW(L"top_golems", 100, 100, LOL_MEM_NETOBJECT_TOP_GOLEMS_PATTERN);
		// TIMER_NEW(L"top_wolves", 100, 100, LOL_MEM_NETOBJECT_TOP_WOLVES_PATTERN);

		// TIMER_NEW(L"bot_blue", 300, 115, LOL_MEM_NETOBJECT_BOT_BLUE_PATTERN);
		// TIMER_NEW(L"bot_red", 300, 115, LOL_MEM_NETOBJECT_BOT_RED_PATTERN);
		// TIMER_NEW(L"bot_wraiths", 100, 100, LOL_MEM_NETOBJECT_BOT_WRAITHS_PATTERN);
		// TIMER_NEW(L"bot_golems", 100, 100, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
		// TIMER_NEW(L"bot_wolves", 100, 100, LOL_MEM_NETOBJECT_BOT_WOLVES_PATTERN);
		LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Timers", L"Summoners Rift timers loaded");
	}

	//Indicators
	SetStatusText(L"JT2: Loading indicators");
	LOG_DEBUG_MF(L"Core.cpp", L"Core", L"Indicators", L"creating...");
	INDICATOR_NEW(L"smite", 420, 25);
	INDICATOR_NEW(L"ignite", 50, 20);
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Indicators", L"loaded");
	
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Objects", L"creation is complete");

	SetStatusText(L"JT2: First init is done");
	return true;
}

void Core::OnDXEndScene(LPDIRECT3DDEVICE9 pDevice){
	LOG_DEBUG_EF_MF(L"Core.cpp", L"Core", L"Render", L"frame catched");

	//Increase special frames counter
	framesCounter %= 10000; //Overflow...
	framesCounter++;

	//Get current in-game time
	//(int)floor(*ingameClockNow - *ingameClockStart)
	int errCode = 0;
	float now = 999.0f;
	float start = 999.0f;
	if((errCode = TryDirectReadMem<float>(&now, ingameClockNow)) != (int)&now){
		LOG_ERROR_CODE_MF(L"Core.cpp", L"Core", L"Render", L"cannot read current time, error code: ", errCode);
		return;
	}
	if((errCode = TryDirectReadMem<float>(&start, ingameClockStart)) != (int)&start){
		LOG_ERROR_CODE_MF(L"Core.cpp", L"Core", L"Render", L"cannot read start time, error code: ", errCode);
		return;
	}
	
	int curTime = (int)floor(now - start);
	LOG_DEBUG_EF_MF(L"Core.cpp", L"Core", L"Render", L"got a time");

	if(curTime < 3){
		LOG_DEBUG_EF_MF(L"Core.cpp", L"Core", L"Render", L"too early to render, skipping");
		return;
	}

	//Go-go!
	for_each(vector<IRenderableObject*>, renderObjects, it)
		(*it)->Render(pDevice, framesCounter, curTime);
	LOG_DEBUG_EF_MF(L"Core.cpp", L"Core", L"Render", L"frame is done");
}

//Calls on the first frame, after the initiation of the core(see flag isInitiated)
bool Core::OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice){
	LOG_DEBUG_MF(L"Core.cpp", L"Core", L"FirstFrame", L"frame catched");
	if(!InitTimePointers()){
		SetStatusText(L"JT2: Cannot load clock memory, see log file.");
		LOG_DEBUG_MF(L"Core.cpp", L"Core", L"FirstFrame", L"cannot read clock memory");
		return false;
	}
	SetStatusText(L"JT2: In-game clock is loaded");

	for_each(vector<IRenderableObject*>, renderObjects, it){
		(*it)->Init(pDevice);
		(*it)->PrepareResources(pDevice);
	}

	SetStatusText(L"JT2: Objects are ready");
	SetStatusIsRender(false);
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"FirstFrame", L"work is done");
	return true;
}

void Core::OnDXResetDevice(LPDIRECT3DDEVICE9 pDevice){
	LOG_DEBUG_MF(L"Core.cpp", L"Core", L"DXReset", L"trying to reset DX resources...");
	for_each(vector<IRenderableObject*>, renderObjects, it)
		(*it)->OnResetDevice(pDevice);
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"DXReset", L"done");
}

void Core::OnDXLostDevice(){
	LOG_DEBUG_MF(L"Core.cpp", L"Core", L"LostDevice", L"trying to free DX resources...");
	for_each(vector<IRenderableObject*>, renderObjects, it)
		(*it)->OnLostDevice();
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"LostDevice", L"done");
}

bool Core::InitTimePointers(){
	LOG_DEBUG_MF(L"Core.cpp", L"Core", L"Pointers", L"loading...");
	int errCode = 0;
	int tmp = 0;

	//Clock pointers, so special...
	if(ingameClockNow == NULL){
		//ingameClockNow = (float *)((*(DWORD*)LOL_MEM_CURRENT_TIME_STRUCT_PTR)+LOL_MEM_CURRENT_TIME_OFFSET);
		if((errCode = TryCopyMem(&tmp, (void*)LOL_MEM_CURRENT_TIME_STRUCT_PTR, sizeof(void*))) != (int)&tmp){
			LOG_ERROR_CODE_MF(L"Core.cpp", L"Core", L"Pointers", L"cannot read in-game now clock memory, error code: ", errCode);
			SetStatusText(L"JT2: Cannot read in-game now clock memory, see log file.");
			return false;
		}
		if(tmp == 0x0){
			LOG_ERROR_MF(L"Core.cpp", L"Core", L"Pointers", L"cannot read in-game now clock memory: struct pointer is null");
			SetStatusText(L"JT2: Cannot read in-game now clock memory, see log file.");
			return false;
		}
		ingameClockNow = (float *)(tmp + LOL_MEM_CURRENT_TIME_OFFSET);
		wchar_t buf[2048];
		swprintf(buf, 2048, L"Current time address: %p->%p+%p->%p = float", LOL_MEM_CURRENT_TIME_STRUCT_PTR, tmp, LOL_MEM_CURRENT_TIME_OFFSET, ingameClockNow);
		LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Pointers", buf);

		//Check value
		float tmp2 = .0f;
		if((errCode = TryCopyMem(&tmp2, (void*)ingameClockNow, sizeof(float))) != (int)&tmp2){
			LOG_ERROR_CODE_MF(L"Core.cpp", L"Core", L"Pointers", L"cannot check in-game now clock memory, error code: ", errCode);
			SetStatusText(L"JT2: Cannot read in-game now clock memory, see log file.");
			return false;
		}
		//-1 in the game loading, 14400 -  4 hours
		if(tmp2 < -1 || tmp2 > 14400){
			SetStatusText(L"JT2: Cannot read in-game now clock memory, see log file.");
			LOG_ERROR_MF(L"Core.cpp", L"Core", L"Pointers", L"it seems, 'now clock' address is broken(< -1 or > 14400)");
			return false;
		}
	}

	if(ingameClockStart == NULL){
		ingameClockStart = (float *)LOL_MEM_START_TIME_PTR;
		wchar_t buf[2048];
		swprintf(buf, 2048, L"Start time address: %p = float", LOL_MEM_START_TIME_PTR);
		LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Pointers", buf);

		//Check value
		float tmp2 = .0f;
		if((errCode = TryCopyMem(&tmp2, (void*)ingameClockStart, sizeof(float))) != (int)&tmp2){
			LOG_ERROR_CODE_MF(L"Core.cpp", L"Core", L"Pointers", L"cannot check in-game start clock memory, error code: ", errCode);
			SetStatusText(L"JT2: Cannot read in-game start clock memory, see log file.");
			return false;
		}
		//-1 in the game loading, 14400 -  4 hours
		if(tmp2 < -1 || tmp2 > 14400){
			SetStatusText(L"JT2: Cannot read in-game start clock memory, see log file.");
			LOG_ERROR_MF(L"Core.cpp", L"Core", L"Pointers", L"it seems, 'start clock' address is broken(< -1 or > 14400)");
			return false;
		}
	}

	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Pointers", L"pointers are found");
	return true;
}

}
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
void Core::OnDXInitiated(void){
	//Check current map for support of our timers
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Objects", L"waiting for the map");
	int tmp = 0;

	//If string is zero-size, it mean map wasnt loaded. Yet. We need to wait some time.
	while((tmp = strlen((char*)LOL_MEM_MAP_NAME_OFFSET)) == 0)
		Sleep(10);
	LOG_DEBUG_MF(L"Core.cpp", L"Core", L"Objects", L"map found");

	//Compare string, just like a usual string, it's just a bytes
	//Summoners Rift and its versions
	if((strcmp(LOL_MAP_SUMMONERS_RIFT_NAME, (char*)LOL_MEM_MAP_NAME_OFFSET) == 0) ||
		(strcmp(LOL_MAP_SUMMONERS_RIFT_AUT_NAME, (char*)LOL_MEM_MAP_NAME_OFFSET) == 0)){
		tmp = LOL_MAP_SUMMONERS_RIFT;
		LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Objects", L"current map is Summoners Rift");
	//Unknown
	}else{
		tmp = 0;
		LOG_ERROR_MF(L"Core.cpp", L"Core", L"Objects", L"unknown map loaded!");
	}

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
	LOG_DEBUG_MF(L"Core.cpp", L"Core", L"Indicators", L"creating...");
	INDICATOR_NEW(L"smite", 420, 25);
	INDICATOR_NEW(L"ignite", 50, 20);
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Indicators", L"loaded");
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Objects", L"creation is complete");

	//Turn off renderring of the status text
	SetStatusIsRender(false);
	isInitiated = true;
}
void Core::OnDXEndScene(LPDIRECT3DDEVICE9 pDevice){
	LOG_DEBUG_EF_MF(L"Core.cpp", L"Core", L"Render", L"frame catched");

	//Increase special frames counter
	framesCounter %= 10000; //Overflow...
	framesCounter++;

	//Get current in-game time
	int curTime = LOL_MEM_GET_GAMETIME();
	LOG_DEBUG_EF_MF(L"Core.cpp", L"Core", L"Render", L"got a time");

	//Go-go!
	for_each(vector<IRenderableObject*>, renderObjects, it)
		(*it)->Render(pDevice, framesCounter, curTime);
	LOG_DEBUG_EF_MF(L"Core.cpp", L"Core", L"Render", L"frame is done");
}

//Calls on the first frame, after the initiation of the core(see flag isInitiated)
void Core::OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice){
	LOG_DEBUG_MF(L"Core.cpp", L"Core", L"FirstFrame", L"frame catched");
	InitTimePointers();

	for_each(vector<IRenderableObject*>, renderObjects, it){
		(*it)->Init(pDevice);
		(*it)->PrepareResources(pDevice);
	}
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"FirstFrame", L"work is done");
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

void Core::InitTimePointers(){
	LOG_DEBUG_MF(L"Core.cpp", L"Core", L"Pointers", L"loading...");
	//Clock pointers, so special...
	if(ingameClockNow == NULL)
		ingameClockNow = (float *)((*(DWORD*)LOL_MEM_CURRENT_TIME_STRUCT_PTR)+LOL_MEM_CURRENT_TIME_OFFSET);
	if(ingameClockStart == NULL)
		ingameClockStart = (float *)LOL_MEM_START_TIME_PTR;
	LOG_VERBOSE_MF(L"Core.cpp", L"Core", L"Pointers", L"pointers are found");
}

}
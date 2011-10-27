#include "defines.h"
#include "Core.h"

#include "ObjectTimer.h"
#include "SmiteIgniteIndicator.h"

namespace JungleTime{

Core* Core::singleton;

Core::Core(void) : DX9Manager(),
	ingameClockNow(NULL), ingameClockStart(NULL){
	LOG_VERBOSE(L"* Core created");
}

Core::~Core(void){
	LOG_VERBOSE(L"* Core close...");
	//Kill it with holy fire!
	for (vector<IRenderableObject*>::iterator it = renderObjects.begin(); it != renderObjects.end(); it++)
		delete *it;

	LOG_VERBOSE(L"* Core closed");
}

int Core::Init(void){
	LOG_VERBOSE(L"* Core initiated");
	return TRUE;
}

#define TIMER_NEW(innname, cd, spawnat, mempattern) renderObjects.push_back(new ObjectTimer(##innname, ##cd, ##spawnat, ##mempattern));
#define INDICATOR_NEW(innname, flat, perlvl) renderObjects.push_back(new SmiteIgniteIndicator(##innname, ##flat, ##perlvl));
void Core::OnDXInitiated(void){
	LOG_VERBOSE(L"Core(Objects): Initiate...");
	//Check current map for support of our timers
	LOG_VERBOSE(L"Core(Objects): waiting for the map...");
	int tmp = 0;
	while((tmp = strlen((char*)LOL_MEM_MAP_NAME_OFFSET)) == 0)
		Sleep(10);
	LOG_VERBOSE(L"Core(Objects): map loaded");
	if((strcmp(LOL_MAP_SUMMONERS_RIFT_NAME, (char*)LOL_MEM_MAP_NAME_OFFSET) == 0) ||
		(strcmp(LOL_MAP_SUMMONERS_RIFT_AUT_NAME, (char*)LOL_MEM_MAP_NAME_OFFSET) == 0)){
		tmp = LOL_MAP_SUMMONERS_RIFT;
		LOG_VERBOSE(L"Core(Objects): current map is Summoners Rift");
	}

	//We support only Summoners Rift map for jungle camps
	if(tmp == LOL_MAP_SUMMONERS_RIFT){
		//Loading all the timers
		LOG_VERBOSE(L"Core(Objects): Initiate...");
		TIMER_NEW(L"baron", 420, 900, LOL_MEM_NETOBJECT_BARON_PATTERN);
		TIMER_NEW(L"dragon", 360, 150, LOL_MEM_NETOBJECT_DRAGON_PATTERN);

		TIMER_NEW(L"top_blue", 300, 115, LOL_MEM_NETOBJECT_TOP_BLUE_PATTERN);
		TIMER_NEW(L"top_red", 300, 115, LOL_MEM_NETOBJECT_TOP_RED_PATTERN);
		TIMER_NEW(L"top_wraiths", 100, 100, LOL_MEM_NETOBJECT_TOP_WRAITHS_PATTERN);
		TIMER_NEW(L"top_golems", 100, 100, LOL_MEM_NETOBJECT_TOP_GOLEMS_PATTERN);
		TIMER_NEW(L"top_wolves", 100, 100, LOL_MEM_NETOBJECT_TOP_WOLVES_PATTERN);

		TIMER_NEW(L"bot_blue", 300, 115, LOL_MEM_NETOBJECT_BOT_BLUE_PATTERN);
		TIMER_NEW(L"bot_red", 300, 115, LOL_MEM_NETOBJECT_BOT_RED_PATTERN);
		TIMER_NEW(L"bot_wraiths", 100, 100, LOL_MEM_NETOBJECT_BOT_WRAITHS_PATTERN);
		TIMER_NEW(L"bot_golems", 100, 100, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
		TIMER_NEW(L"bot_wolves", 100, 100, LOL_MEM_NETOBJECT_BOT_WOLVES_PATTERN);
		LOG_VERBOSE(L"Core(Timers): done");
	}

	//Indicators
	INDICATOR_NEW(L"smite", 420, 25);
	INDICATOR_NEW(L"ignite", 50, 20);
	LOG_VERBOSE(L"Core(Indicators): done");

	isInitiated = true;
}
void Core::OnDXEndScene(LPDIRECT3DDEVICE9 pDevice){
	//Increase special frames counter
	framesCounter %= 100; //Overflow...
	framesCounter++;

	//Get current in-game time
	int curTime = LOL_MEM_GET_GAMETIME();

	//Go-go!
	for (vector<IRenderableObject*>::iterator it = renderObjects.begin(); it != renderObjects.end(); it++)
		(*it)->Render(pDevice, framesCounter, curTime);
	LOG_VERBOSE(L"Core: frame...");
}
void Core::OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice){
	LOG_VERBOSE(L"Core: first frame");
	InitTimePointers();

	for (vector<IRenderableObject*>::iterator it = renderObjects.begin(); it != renderObjects.end(); it++)
		(*it)->PrepareRender(pDevice);
	LOG_VERBOSE(L"Core: first frame done");
}

void Core::InitTimePointers(){
	//Clock pointers, so special...
	if(ingameClockNow == NULL)
		ingameClockNow = (float *)((*(DWORD*)LOL_MEM_CURRENT_TIME_STRUCT_PTR)+LOL_MEM_CURRENT_TIME_OFFSET);
	if(ingameClockStart == NULL)
		ingameClockStart = (float *)LOL_MEM_START_TIME_PTR;
}

}
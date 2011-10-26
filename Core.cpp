#include "defines.h"
#include "Core.h"

namespace JungleTime{

Core* Core::singleton;

Core::Core(void) : DX9Manager(), isEnabled(true),
	ingameClockNow(NULL), ingameClockStart(NULL){
	LOG_VERBOSE(L"* Core created");
}

Core::~Core(void){
	//Kill it with holy fire!
	for (vector<ObjectTimer*>::iterator it = timers.begin(); it != timers.end(); it++)
		delete *it;

	LOG_VERBOSE(L"* Core closed");
}

int Core::Init(void){
	LOG_VERBOSE(L"* Core initiated");
	return TRUE;
}

#define TIMER_NEW(innname, cd, spawnat, mempattern) timers.push_back(new ObjectTimer(##innname, ##cd, ##spawnat, ##mempattern));
void Core::OnDXInitiated(void){
	//Loading all the timers
	LOG_VERBOSE(L"Core: Timers: Initiate...");
	TIMER_NEW(L"baron", 420, 900, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"dragon", 360, 150, LOL_MEM_NETOBJECT_DRAGON_PATTERN);

	TIMER_NEW(L"top_blue", 300, 115, LOL_MEM_NETOBJECT_TOP_BLUE_PATTERN);
	TIMER_NEW(L"top_red", 300, 115, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"top_wraiths", 100, 100, LOL_MEM_NETOBJECT_TOP_WRAITHS_PATTERN);
	TIMER_NEW(L"top_golems", 100, 100, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"top_wolves", 100, 100, LOL_MEM_NETOBJECT_TOP_WOLVES_PATTERN);

	TIMER_NEW(L"bot_blue", 300, 115, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"bot_red", 300, 115, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"bot_wraiths", 100, 100, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"bot_golems", 100, 100, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"bot_wolves", 100, 100, LOL_MEM_NETOBJECT_BOT_WOLVES_PATTERN);
	
	isInitiated = true;
	LOG_VERBOSE(L"Core(Timers): done");
}
void Core::OnDXEndScene(LPDIRECT3DDEVICE9 pDevice){
	//Increase special frames counter
	framesCounter %= 100; //Overflow...
	framesCounter++;

	//Get current in-game time
	int curTime = LOL_MEM_GET_GAMETIME();

	//Go-go!
	for (vector<ObjectTimer*>::iterator it = timers.begin(); it != timers.end(); it++)
		(*it)->Render(pDevice, framesCounter, curTime);
}
void Core::OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice){
	InitTimePointers();

	for (vector<ObjectTimer*>::iterator it = timers.begin(); it != timers.end(); it++)
		(*it)->PrepareRender(pDevice);
}

void Core::InitTimePointers(){
	//Clock pointers, so special...
	if(ingameClockNow == NULL)
		ingameClockNow = (float *)((*(DWORD*)LOL_MEM_CURRENT_TIME_STRUCT_PTR)+LOL_MEM_CURRENT_TIME_OFFSET);
	if(ingameClockStart == NULL)
		ingameClockStart = (float *)LOL_MEM_START_TIME_PTR;
}

}
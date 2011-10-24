#include "defines.h"
#include "Core.h"

namespace JungleTime{

Core* Core::singleton;

Core::Core(void) : DX9Manager(), isEnabled(true){
	LOG_VERBOSE(L"* Core created");
}

Core::~Core(void){
	LOG_VERBOSE(L"* Core closed");
}

int Core::Init(void){
	LOG_VERBOSE(L"* Core initiated");
	return TRUE;
}

#define TIMER_NEW(innname, cd, spawnat, mempattern) timers.push_back(boost::shared_ptr<ObjectTimer>(new ObjectTimer(##innname, ##cd, ##spawnat, ##mempattern, resConfig)));
void Core::OnDXInitiated(void){
	//Our config for the current resolution
	boost::property_tree::ini_parser::read_ini("jt2_resconf_cur.ini", resConfig);

	//Loading all the timers
	LOG_VERBOSE(L"Core: Timers: Initiate...");
	TIMER_NEW(L"baron", 420, 900, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"dragon", 360, 150, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);

	TIMER_NEW(L"top_blue", 300, 115, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"top_red", 300, 115, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"top_wraiths", 100, 100, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"top_golems", 100, 100, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"top_wolves", 100, 100, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);

	TIMER_NEW(L"bot_blue", 300, 115, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"bot_red", 300, 115, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"bot_wraiths", 100, 100, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"bot_golems", 100, 100, LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN);
	TIMER_NEW(L"bot_wolves", 100, 100, LOL_MEM_NETOBJECT_BOT_WOLVES_PATTERN);
	
	isInitiated = true;
	LOG_VERBOSE(L"Core(Timers): done");
}
void Core::OnDXEndScene(LPDIRECT3DDEVICE9 pDevice){
	for (vector<boost::shared_ptr<ObjectTimer>>::iterator it = timers.begin(); it != timers.end(); it++)
		(*it)->Render(pDevice);
}
void Core::OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice){
	for (vector<boost::shared_ptr<ObjectTimer>>::iterator it = timers.begin(); it != timers.end(); it++)
		(*it)->PrepareRender(pDevice);
}

}
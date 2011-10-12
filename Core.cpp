#include "defines.h"
#include "Core.h"

namespace JungleTime{

Core* Core::singleton;

Core::Core(void) : DX9Manager(){
	LOG_VERBOSE(L"* Core created");
}

Core::~Core(void){
	LOG_VERBOSE(L"* Core closed");
}

int Core::Init(void){
	LOG_VERBOSE(L"* Core initiated");
	return TRUE;
}

#define TIMER_NEW(innname, outname, cd) timers.push_back(boost::shared_ptr<ObjectTimer>(new ObjectTimer(##innname, ##outname, ##cd)));
void Core::OnDXInitiated(void){
	//Our config for the current resolution
	boost::property_tree::ini_parser::read_ini("jt2_res_config.ini", resConfig);

	//Loading all the timers
	TIMER_NEW("baron", "Baron", 420);
	TIMER_NEW("dragon", "Dragon", 360);

	TIMER_NEW("top_blue", "Top Blue", 300);
	TIMER_NEW("top_red", "Top Red", 300);
	TIMER_NEW("top_wraiths", "Top Wraiths", 100);
	TIMER_NEW("top_golems", "Top Golems", 100);
	TIMER_NEW("top_wolves", "Top Wolves", 100);

	TIMER_NEW("bot_blue", "Bot Blue", 300);
	TIMER_NEW("bot_red", "Bot Red", 300);
	TIMER_NEW("bot_wraiths", "Bot Wraiths", 100);
	TIMER_NEW("bot_golems", "Bot Golems", 100);
	TIMER_NEW("bot_wolves", "Bot Wolves", 100);
}
void Core::OnDXEndScene(LPDIRECT3DDEVICE9 pDevice){

}
void Core::OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice){

}

}
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

#define TIMER_NEW(innname, cd) timers.push_back(boost::shared_ptr<ObjectTimer>(new ObjectTimer(##innname, ##cd, resConfig)));
void Core::OnDXInitiated(void){
	//Our config for the current resolution
	boost::property_tree::ini_parser::read_ini("jt2_resconf_cur.ini", resConfig);

	//Loading all the timers
	LOG_VERBOSE(L"Core: Timers: Initiate...");
	TIMER_NEW(L"baron", 420);
	TIMER_NEW(L"dragon", 360);

	TIMER_NEW(L"top_blue", 300);
	TIMER_NEW(L"top_red", 300);
	TIMER_NEW(L"top_wraiths", 100);
	TIMER_NEW(L"top_golems", 100);
	TIMER_NEW(L"top_wolves", 100);

	TIMER_NEW(L"bot_blue", 300);
	TIMER_NEW(L"bot_red", 300);
	TIMER_NEW(L"bot_wraiths", 100);
	TIMER_NEW(L"bot_golems", 100);
	TIMER_NEW(L"bot_wolves", 100);
	isInitiated = true;
	LOG_VERBOSE(L"Core: Timers: done");

	//Now Jungle Timer starts rendering timers and labels, we can start pixel-matching
	HDC hdc = 0;
	HWND hwnd = 0;
	//Wait for the LoL window
	LOG_VERBOSE(L"Core: Waiting for the window");
	while(!(hwnd = FindWindow(NULL, LOL_WINDOW_NAME)))
		Sleep(50);

	LOG_VERBOSE(L"Core: Window found!");
	hdc = GetDC(hwnd);

	//Go-go, pawa rangers!
	LOG_VERBOSE(L"Core: Loop started");
	while(isEnabled){
		for (vector<boost::shared_ptr<ObjectTimer>>::iterator it = timers.begin(); it != timers.end(); it++)
			(*it)->CheckPixels(hdc);
		Sleep(1000);
	}

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
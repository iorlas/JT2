#include "ObjectTimer.h"

namespace JungleTime{

ObjectTimer::ObjectTimer(LPCWSTR innerName, int cooldown, int spawnAt, int objectMemoryPattern)
	: innerName(innerName), cooldown(cooldown), spawnAt(spawnAt), objectMemoryPattern(objectMemoryPattern),
	killedAt(0), isAlivePtr(0), isAliveBefore(false),
	timerFont(0){
	LOG_DEBUG_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"loading...");

	/************************************************************************/
	/* OVERLAY                                                              */
	/************************************************************************/
	//Label
	labelName = INIReadStr(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_label_name"), CONFIG_NAME_DESIGN);
	showLabel = !!INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_show_label"), CONFIG_NAME_DESIGN);
	SetRect(&labelCoords,
		INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_label_pos_x"), CONFIG_NAME_DESIGN),
		INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_label_pos_y"), CONFIG_NAME_DESIGN),
		300, 300);

	//Timer
	timerFontSize = INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_timer_font_size"), CONFIG_NAME_DESIGN);
	timerFontWeight = INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_timer_font_weight"), CONFIG_NAME_DESIGN);
	timerFontName = INIReadStr(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_timer_font_name"), CONFIG_NAME_DESIGN);
	timerFontColor = INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_timer_font_color"), CONFIG_NAME_DESIGN);
	timerFontRedlineColor = INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_timer_font_redline_color"), CONFIG_NAME_DESIGN);
	timerFontAliveColor = INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_timer_font_alive_color"), CONFIG_NAME_DESIGN);

	redLine = INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_red_line"), CONFIG_NAME_DESIGN);
	SetRect(&timerCoords,
		INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_timer_pos_x"), CONFIG_NAME_DESIGN),
		INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_timer_pos_y"), CONFIG_NAME_DESIGN),
		300, 300);

	//Announce
	//TODO: Not implemented yet, so we don't need to load it.
	/*showAnnounce = config.get<bool>("overlay."+innerNameStr+"_show_announce");
	announceFontSize = config.get<int>("overlay."+innerNameStr+"_announce_font_size");
	announceFontWeight = config.get<int>("overlay."+innerNameStr+"_announce_font_weight");
	announceFontName = StringToWString(config.get<string>("overlay."+innerNameStr+"_announce_font_name"));
	announceFontColor = config.get<DWORD>("overlay."+innerNameStr+"_announce_font_color");
	SetRect(&announceCoords,
		config.get<int>("overlay."+innerNameStr+"_announce_pos_x"),
		config.get<int>("overlay."+innerNameStr+"_announce_pos_y"),
		300, 300);*/
	
	LOG_VERBOSE_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"loaded");
}

void ObjectTimer::Render(PDIRECT3DDEVICE9 pDevice, int frameNum, int curTimeSecs){
	LOG_DEBUG_EF_MF(L"ObjectTimer.cpp", curTimeSecs, L"ObjectTimers", innerName, L"drawing");

	//Every 10 frames we'll try to find net-objects
	// !isAlivePtr - if this pointer isn't null, it means we already have all pointers
	// (curTimeSecs > spawnAt-1) - we dont need to check for object before it CAN BE spawned.
	// frameNum%10 == 0 - we cant check all things every frame, cause it is unneccesery and can overload LoL graphics engine
	if(!isAlivePtr && (curTimeSecs > (spawnAt-1)) && frameNum%100 == 0){
		TryToInitNetobjectPointers();
		if(!isAlivePtr && (curTimeSecs > spawnAt+30)){
			LOG_ERROR_MF(L"ObjectTimer.cpp", curTimeSecs, L"ObjectTimers", innerName, L"isAlive pointer is broken(still not found for a spawn time +30sec)");
		}
	}

	//Show label only if we need
	if(showLabel){
		//0xFFFFFFFF
		timerFont->DrawText(NULL, labelName, -1, &labelCoords, DT_NOCLIP, timerFontColor);
	}

	//If object isn't spawned yet, we need to display time, left to first spawn
	if(TIMER_IS_ALIVE){
		//Now it's alive!
		if(!isAliveBefore){
			isAliveBefore = true;
			LOG_VERBOSE_MF(L"ObjectTimer.cpp", curTimeSecs, L"ObjectTimers", innerName, L"is alive!");
		}

		//0xFFB5E61D
		timerFont->DrawText(NULL, TIMER_LABEL_ALIVE, -1, &timerCoords, DT_NOCLIP, timerFontAliveColor);
	}else{
		//Now it's dead...
		if(isAliveBefore){
			killedAt = curTimeSecs;
			isAliveBefore = false;
			LOG_VERBOSE_MF(L"ObjectTimer.cpp", curTimeSecs, L"ObjectTimers", innerName, L"is dead!");
		}
		
		//Get time to [next]spawn
		int timeLeft = TIMER_IS_NOT_SPAWNED? spawnAt - curTimeSecs : cooldown - (curTimeSecs - killedAt);
		
		//I don't want to show you a negative numbers
		timeLeft = timeLeft < 0 ? 0 : timeLeft;

		//Fast convert to wstring
		wchar_t istr[32];
		_itow_s(timeLeft, istr, 10);

		//Highlight if we have small time to get it alive
		if(timeLeft <= redLine)
			//0xFFFFC90E
			timerFont->DrawText(NULL, istr, -1, &timerCoords, DT_NOCLIP, timerFontRedlineColor);
		else
			//0xFFFFFFFF
			timerFont->DrawText(NULL, istr, -1, &timerCoords, DT_NOCLIP, timerFontColor);
	}

	LOG_DEBUG_EF_MF(L"ObjectTimer.cpp", curTimeSecs, L"ObjectTimers", innerName, L"draw is done");
}

bool ObjectTimer::Init(PDIRECT3DDEVICE9 pDevice){
	bool res = true;
	LOG_DEBUG_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"preparing...");
	//Gather pointers to needed related information from the game
	/*res = res && */TryToInitNetobjectPointers();
	LOG_VERBOSE_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"is ready");
	return res;
}
bool ObjectTimer::PrepareResources(PDIRECT3DDEVICE9 pDevice){
	//Create DirectX objects
	HRESULT res = D3DXCreateFont(pDevice, timerFontSize, 0, timerFontWeight, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, timerFontName, &timerFont);
	if(res != S_OK){
		LOG_DEBUG_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"cannot create resources");
		return false;
	}
	LOG_DEBUG_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"resources are ready");
	return true;
}

void ObjectTimer::OnLostDevice(){
	if(timerFont)
		timerFont->OnLostDevice();
}

void ObjectTimer::OnResetDevice(PDIRECT3DDEVICE9 pDevice){
	if(timerFont)
		timerFont->OnResetDevice();
}

bool ObjectTimer::TryToInitNetobjectPointers(){
	LOG_DEBUG_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"trying to find needed pointers");

	int errCode = 0;
	DWORD arr = NULL, tmp2 = NULL;//for teh logging

	//LOL_MEM_PLAYER_LEVEL_PTR->
	if((errCode = TryCopyMem(&arr, (void*)LOL_MEM_NETOBJECTS_ARRAY_PTR, sizeof(void*))) != (int)&arr){
		LOG_ERROR_CODE_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"cannot read netobjects array memory, error code: ", errCode);
		return false;
	}
	if(arr == NULL){
		LOG_ERROR_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"cannot read netobjects array memory: pointer to array is Null");
		return false;
	}
	for(int i = 0; i < LOL_MEM_NETOBJECTS_MAX; i++){
		DWORD obj = NULL;
		if((errCode = TryCopyMem(&obj, (void*)(arr+i*4), sizeof(void*))) != (int)&obj){
			LOG_ERROR_CODE_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"cannot read netobject memory, error code: ", errCode);
			return false;
		}
		//DWORD obj = ((DWORD*)arr)[i];
		if(obj == 0x00000000)
			continue;
		
		//First part check
		int memPattern = 0;
		if((errCode = TryCopyMem(&memPattern, (void*)(obj+LOL_MEM_NETOBJECT_PATTERN_OFFSET), sizeof(int))) != (int)&memPattern){
			LOG_ERROR_CODE_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"cannot read netobjects pattern part memory, error code: ", errCode);
			continue;
		}
		if(memPattern != objectMemoryPattern)
			continue;
		
		//Second part check
		memPattern = 0;
		if((errCode = TryCopyMem(&memPattern, (void*)(obj+LOL_MEM_NETOBJECT_PATTERN2_OFFSET), sizeof(int))) != (int)&memPattern){
			LOG_ERROR_CODE_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"cannot read netobjects pattern 2 part memory, error code: ", errCode);
			continue;
		}
		if(memPattern != objectMemoryPattern)
			continue;

		//Check isAlive flag
		byte isAlive = 0;
		if((errCode = TryCopyMem(&isAlive, (void*)(obj+LOL_MEM_NETOBJECT_IS_ALIVE_OFFSET), sizeof(bool))) != (int)&isAlive){
			LOG_ERROR_CODE_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"cannot read isAlive part of verified netobject memory, error code: ", errCode);
			return false;
		}
		if(isAlive != 0x01 && isAlive != 0x00){
			LOG_ERROR_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"pointers are broken! isAlive flag != 0 || 1");
		}

		isAlivePtr = (bool *)(obj+LOL_MEM_NETOBJECT_IS_ALIVE_OFFSET);

		wchar_t buf[2048];
		swprintf(buf, 2048, L"Netobject isAlive address: %p->%p+%p*4->%p+%p->%p = bool", LOL_MEM_NETOBJECTS_ARRAY_PTR, arr, i, obj, LOL_MEM_NETOBJECT_IS_ALIVE_OFFSET, isAlivePtr);
		LOG_VERBOSE_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, buf);
		return true;
	}
	LOG_DEBUG_EF_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"pointers not found!");
	return false;
}

ObjectTimer::~ObjectTimer(void){
	LOG_DEBUG_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"unloading...");

	//Delete allocated mem
	delete labelName;
	delete timerFontName;

	//Release resources only if application loop is alive, we dont need to freeze application in the process list
	if(timerFont != NULL && !IS_DX_LOOP_DEAD)
		timerFont->Release();
	LOG_VERBOSE_MF(L"ObjectTimer.cpp", 0, L"ObjectTimers", innerName, L"unloaded");	
}

}
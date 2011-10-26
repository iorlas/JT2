#include "ObjectTimer.h"

wstring StringToWString(const string& s){
	wstring temp(s.length(),L' ');
	copy(s.begin(), s.end(), temp.begin());
	return temp; 
}

wstring INIReadStr(wstring cat, wstring name, wstring fileName){
	WCHAR buffer[1024];
	int res = GetPrivateProfileString(cat.c_str(), name.c_str(), L"NONE_STR", buffer, -1, fileName.c_str());
	if(res == -2 || res == -3){
		LOG_VERBOSE((L"INI Loader: cannot load string " + cat + L"." + name + L": buffer too small").c_str());
		return L"ERR1";
	}
	if(res == -1){
		res = GetLastError();
		if(res == 0x2){
			LOG_VERBOSE((L"INI Loader: cannot load string " + cat + L"." + name + L": config file not found").c_str());
			return L"ERR404";
		}
	}
	return wstring(buffer);
}

int INIReadInt(wstring cat, wstring name, wstring fileName){
	int res = GetPrivateProfileInt(cat.c_str(), name.c_str(), -1, fileName.c_str());
	if(GetLastError() == 0x2)
		LOG_VERBOSE((L"INI Loader: cannot load int " + cat + L"." + name + L": config file not found").c_str());
	return res;
}

namespace JungleTime{

ObjectTimer::ObjectTimer(wstring innerName, int cooldown, int spawnAt, int objectMemoryPattern)
	: innerName(innerName), cooldown(cooldown), spawnAt(spawnAt), objectMemoryPattern(objectMemoryPattern),
	killedAt(0), isAlivePtr(0), isAliveBefore(false){
	LOG_VERBOSE((L"Object Timers: Loading " + innerName).c_str());

	/************************************************************************/
	/* OVERLAY                                                              */
	/************************************************************************/
	//Label
	labelName = INIReadStr(L"overlay", innerName+L"_label_name", CONFIG_NAME_DESIGN);
	showLabel = !!INIReadInt(L"overlay", innerName+L"_show_label", CONFIG_NAME_DESIGN);
	SetRect(&labelCoords,
		INIReadInt(L"overlay", innerName+L"_label_pos_x", CONFIG_NAME_DESIGN),
		INIReadInt(L"overlay", innerName+L"_label_pos_y", CONFIG_NAME_DESIGN),
		300, 300);

	//Timer
	timerFontSize = INIReadInt(L"overlay", innerName+L"_timer_font_size", CONFIG_NAME_DESIGN);
	timerFontWeight = INIReadInt(L"overlay", innerName+L"_timer_font_weight", CONFIG_NAME_DESIGN);
	timerFontName = INIReadStr(L"overlay", innerName+L"_timer_font_name", CONFIG_NAME_DESIGN);
	timerFontColor = INIReadInt(L"overlay", innerName+L"_timer_font_color", CONFIG_NAME_DESIGN);
	timerFontRedlineColor = INIReadInt(L"overlay", innerName+L"_timer_font_redline_color", CONFIG_NAME_DESIGN);
	timerFontAliveColor = INIReadInt(L"overlay", innerName+L"_timer_font_alive_color", CONFIG_NAME_DESIGN);

	redLine = INIReadInt(L"overlay", innerName+L"_red_line", CONFIG_NAME_DESIGN);
	SetRect(&timerCoords,
		INIReadInt(L"overlay", innerName+L"_timer_pos_x", CONFIG_NAME_DESIGN),
		INIReadInt(L"overlay", innerName+L"_timer_pos_y", CONFIG_NAME_DESIGN),
		300, 300);

	//Announce
	//NODO: Not implemented yet, so we don't need to load it.
	/*showAnnounce = config.get<bool>("overlay."+innerNameStr+"_show_announce");
	announceFontSize = config.get<int>("overlay."+innerNameStr+"_announce_font_size");
	announceFontWeight = config.get<int>("overlay."+innerNameStr+"_announce_font_weight");
	announceFontName = StringToWString(config.get<string>("overlay."+innerNameStr+"_announce_font_name"));
	announceFontColor = config.get<DWORD>("overlay."+innerNameStr+"_announce_font_color");
	SetRect(&announceCoords,
		config.get<int>("overlay."+innerNameStr+"_announce_pos_x"),
		config.get<int>("overlay."+innerNameStr+"_announce_pos_y"),
		300, 300);*/

	LOG_VERBOSE((L"Object Timers: Loading " + innerName + L" - done").c_str());
}

void ObjectTimer::Render(PDIRECT3DDEVICE9 pDevice, int frameNum, int curTimeSecs){
	//Every 10 frames we'll try to find net-objects
	// !isAlivePtr - if this pointer isn't null, it means we already have all pointers
	// (curTimeSecs > spawnAt-5) - we dont need to check for object before it CAN BE spawned. So 3 is a difference.
	// frameNum%10 == 0 - we cant check all things every frame, cause it is unneccesery and can overload LoL graphics engine
	if(!isAlivePtr && (curTimeSecs > spawnAt-3) && frameNum%10 == 0)
		TryToInitNetobjectPointers();

	//Show label only if we need
	if(showLabel)
		//0xFFFFFFFF
		timerFont->DrawText(NULL, labelName.c_str(), -1, &labelCoords, DT_NOCLIP, timerFontColor);
	
	//If object isn't spawned yet, we need to display time, left to first spawn
	if(TIMER_IS_ALIVE){
		//Now it's alive!
		if(!isAliveBefore)
			isAliveBefore = true;

		//0xFFB5E61D
		timerFont->DrawText(NULL, TIMER_LABEL_ALIVE, -1, &timerCoords, DT_NOCLIP, timerFontAliveColor);
	}else{
		//Now it's dead...
		if(isAliveBefore){
			killedAt = curTimeSecs;
			isAliveBefore = false;
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
}

void ObjectTimer::PrepareRender(PDIRECT3DDEVICE9 pDevice){
	//Create DirectX objects
	D3DXCreateFont(pDevice, timerFontSize, 0, timerFontWeight, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, timerFontName.c_str(), &timerFont);

	//Gather pointers to needed related information from the game
	TryToInitNetobjectPointers();
}

void ObjectTimer::TryToInitNetobjectPointers(){
	if(isAlivePtr)
		return;
	DWORD arr = *((DWORD*)(LOL_MEM_NETOBJECTS_ARRAY_PTR));
	DWORD obj = 0;
	for(int i = 0; i < LOL_MEM_NETOBJECTS_MAX; i++){
		//Get object itself
		obj = ((DWORD*)arr)[i];
		if(obj == 0x00000000)
			continue;

		//Grab memory part			
		int* memSrc = (int *)(obj+LOL_MEM_NETOBJECT_PATTERN_OFFSET);

		//Not this one...
		if(!memSrc)
			continue;

		//Check for memory object pattern(see defines.h)
		if (*memSrc == objectMemoryPattern){
			LOG_VERBOSE((L"Object Timers(" + innerName + L"): network object found").c_str());

			//Update our saves pointers
			isAlivePtr = (bool *)(obj+LOL_MEM_NETOBJECT_IS_ALIVE_OFFSET);
			if(!isAlivePtr){
				LOG_VERBOSE((L"Object Timers(" + innerName + L"): cannot find isAlive flag. Memory corrupted? Different game version? Humster in the PC?").c_str());
				continue;
			}

		}
	}
}

ObjectTimer::~ObjectTimer(void){
	//TODO: Release temp resources
	//BUG: If application closes, we cant release any resource. Perhaps, because of loop, it never runs after end of app.
	//timerFont->Release();
}

}
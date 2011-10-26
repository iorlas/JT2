#pragma once

#include "defines.h"

#define TIMER_IS_ALIVE (isAlivePtr? *isAlivePtr: false)

#define TIMER_IS_NOT_SPAWNED (!isAlivePtr)

#define TIMER_LABEL_ALIVE L"ALIVE!"

namespace JungleTime{

class ObjectTimer
{
public:
	ObjectTimer(wstring innerName, int cooldown, int spawnAt, int objectMemoryPattern);
	virtual ~ObjectTimer(void);

	void Render(PDIRECT3DDEVICE9 pDevice, int frameNum, int curTimeSecs);
	void PrepareRender(PDIRECT3DDEVICE9 pDevice);

	//Get pointers we can grab only in runtime, after some special in-game events
	void TryToInitNetobjectPointers();

	//In-game pointers
	int objectMemoryPattern;
	bool *isAlivePtr;

	//We need to know what was before, to save time in the right moment
	bool isAliveBefore;

	//Name for config and logging
	wstring innerName;
	
	int killedAt;
	int cooldown;
	int spawnAt;

	/************************************************************************/
	/*    OVERLAY SETTINGS                                                  */
	/************************************************************************/
	//Label
	wstring labelName;
	bool showLabel;
	RECT labelCoords;
	
	//Timer
	int redLine;
	RECT timerCoords;
	int timerFontSize;
	int timerFontWeight;
	wstring timerFontName;
	DWORD timerFontColor;
	DWORD timerFontRedlineColor;
	DWORD timerFontAliveColor;

	//Announce
	bool showAnnounce;
	RECT announceCoords;
	int announceFontSize;
	int announceFontWeight;
	wstring announceFontName;
	DWORD announceFontColor;

	//Overlay fonts
	LPD3DXFONT timerFont;
};

}
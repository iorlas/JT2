#pragma once

#include "defines.h"

#include "IRenderable.h"

#define TIMER_IS_ALIVE (!!isAlivePtr? *isAlivePtr: false)
#define TIMER_IS_NOT_SPAWNED (!isAlivePtr)
#define TIMER_LABEL_ALIVE L"ALIVE"

namespace JungleTime{

class ObjectTimer : public JungleTime::IRenderableObject
{
public:
	ObjectTimer(LPCWSTR innerName, int cooldown, int spawnAt, int objectMemoryPattern);
	virtual ~ObjectTimer(void);

	void Render(PDIRECT3DDEVICE9 pDevice, int frameNum, int curTimeSecs);
	bool Init(PDIRECT3DDEVICE9 pDevice);

	//DX resources managment
	bool PrepareResources(PDIRECT3DDEVICE9 pDevice);
	void OnLostDevice();
	void OnResetDevice(PDIRECT3DDEVICE9 pDevice);

	//Get pointers we can grab only in runtime, after some special in-game events
	bool TryToInitNetobjectPointers();

	//In-game pointers
	int objectMemoryPattern;
	bool *isAlivePtr;

	//We need to know what was before, to save time in the right moment
	bool isAliveBefore;

	//Name for config and logging
	LPCWSTR innerName;
	
	int killedAt;
	int cooldown;
	int spawnAt;

	/************************************************************************/
	/*    OVERLAY SETTINGS                                                  */
	/************************************************************************/
	//Label
	LPCWSTR labelName;
	bool showLabel;
	RECT labelCoords;
	
	//Timer
	int redLine;
	RECT timerCoords;
	int timerFontSize;
	int timerFontWeight;
	LPCWSTR timerFontName;
	DWORD timerFontColor;
	DWORD timerFontRedlineColor;
	DWORD timerFontAliveColor;

	//Announce
	bool showAnnounce;
	RECT announceCoords;
	int announceFontSize;
	int announceFontWeight;
	LPCWSTR announceFontName;
	DWORD announceFontColor;

	//Overlay fonts
	LPD3DXFONT timerFont;
};

}
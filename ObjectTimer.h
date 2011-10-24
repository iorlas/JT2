#pragma once

#include "defines.h"

#define OBJECT_TIMER_PATTERN_SIZE 4
#define TIMER_LABEL_ALIVE L"ALIVE!"

namespace JungleTime{

class ObjectTimer
{
public:
	ObjectTimer(wstring innerName, double cooldown, double spawnAt, int objectMemoryPattern, boost::property_tree::ptree config);
	virtual ~ObjectTimer(void);

	void Render(PDIRECT3DDEVICE9 pDevice);
	void PrepareRender(PDIRECT3DDEVICE9 pDevice);

	void Start();
	void Stop();

	bool isAlive;

	//In-game pointers
	int objectMemoryPattern;
	bool *isAlivePtr;

	bool isFirstSpawned;

	boost::mutex timerMutex;
	boost::timer mainTimer;
	wstring innerName;
	wstring labelName;
	double cooldown;
	double spawnAt;
	double redLine;

	//overlay settings
	bool showLabel;
	RECT labelCoords;
	
	RECT timerCoords;
	int timerFontSize;
	int timerFontWeight;
	wstring timerFontName;
	DWORD timerFontColor;
	DWORD timerFontRedlineColor;
	DWORD timerFontAliveColor;

	bool showAnnounce;
	RECT announceCoords;
	int announceFontSize;
	int announceFontWeight;
	wstring announceFontName;
	DWORD announceFontColor;

	LPD3DXFONT timerFont;
};

}
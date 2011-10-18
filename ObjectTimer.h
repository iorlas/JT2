#pragma once

#include "defines.h"

#define OBJECT_TIMER_PATTERN_SIZE 4
#define TIMER_LABEL_ALIVE L"ALIVE!"

namespace JungleTime{

class ObjectTimer
{
public:
	ObjectTimer(wstring innerName, double cooldown, boost::property_tree::ptree config);
	virtual ~ObjectTimer(void);

	void Render(PDIRECT3DDEVICE9 pDevice);
	void PrepareRender(PDIRECT3DDEVICE9 pDevice);

	void Start();
	void Stop();

	void CheckPixels(HDC windowDC);

	bool isAlive;

	boost::mutex timerMutex;
	boost::timer mainTimer;
	wstring innerName;
	wstring labelName;
	double cooldown;
	double redLine;
	COLORREF patternLight[OBJECT_TIMER_PATTERN_SIZE*OBJECT_TIMER_PATTERN_SIZE];
	COLORREF patternShadow[OBJECT_TIMER_PATTERN_SIZE*OBJECT_TIMER_PATTERN_SIZE];
	COLORREF patternAvail[OBJECT_TIMER_PATTERN_SIZE*OBJECT_TIMER_PATTERN_SIZE];
	int coords[2];

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
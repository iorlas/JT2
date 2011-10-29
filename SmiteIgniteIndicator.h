#pragma once

#include "defines.h"

#include "IRenderable.h"

#define TYPE_SMITE 1
#define TYPE_IGNITE 2

//Functions for reading from INI from ObjectTimer.cpp
//TODO: make separated file for helper functions
extern wstring INIReadStr(wstring cat, wstring name, wstring fileName);
extern int INIReadInt(wstring cat, wstring name, wstring fileName);

namespace JungleTime{

class SmiteIgniteIndicator : public JungleTime::IRenderableObject
{
public:
	SmiteIgniteIndicator(wstring innerName, int flat, int perlvl)
		: innerName(innerName), flat(flat), perlvl(perlvl){
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName.c_str(), L"loading...");

		/************************************************************************/
		/* OVERLAY                                                              */
		/************************************************************************/
		//Label
		labelName = INIReadStr(L"overlay", innerName+L"_label_name", CONFIG_NAME_DESIGN);
		showLabel = !!INIReadInt(L"overlay", innerName+L"_show_label", CONFIG_NAME_DESIGN);
		labelFontColor = INIReadInt(L"overlay", innerName+L"_label_font_color", CONFIG_NAME_DESIGN);
		SetRect(&labelCoords,
			INIReadInt(L"overlay", innerName+L"_label_pos_x", CONFIG_NAME_DESIGN),
			INIReadInt(L"overlay", innerName+L"_label_pos_y", CONFIG_NAME_DESIGN),
			300, 300);

		//Indicator
		indiFontSize = INIReadInt(L"overlay", innerName+L"_indi_font_size", CONFIG_NAME_DESIGN);
		indiFontWeight = INIReadInt(L"overlay", innerName+L"_indi_font_weight", CONFIG_NAME_DESIGN);
		indiFontName = INIReadStr(L"overlay", innerName+L"_indi_font_name", CONFIG_NAME_DESIGN);
		indiFontColor = INIReadInt(L"overlay", innerName+L"_indi_font_color", CONFIG_NAME_DESIGN);

		SetRect(&indiCoords,
			INIReadInt(L"overlay", innerName+L"_indi_pos_x", CONFIG_NAME_DESIGN),
			INIReadInt(L"overlay", innerName+L"_indi_pos_y", CONFIG_NAME_DESIGN),
			300, 300);
		
		LOG_VERBOSE_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName.c_str(), L"loaded");
	}

	virtual ~SmiteIgniteIndicator(void){
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName.c_str(), L"unloading...");	
		
		//Release resources only if application loop is alive, we dont need to freeze application in the process list
		if(!IS_DX_LOOP_DEAD)
			font->Release();

		LOG_VERBOSE_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName.c_str(), L"unloaded");	
	}

	void Render(PDIRECT3DDEVICE9 pDevice, int frameNum, int curTimeSecs){
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName.c_str(), L"drawing");
		if(!playerLevel)
			TryToInitPointers();

		//If we still cannot find needed pointers - we have nothing to render...
		if(!playerLevel){
			LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName.c_str(), L"pointers still not available");
			return;
		}
		
		wchar_t istr[30];
		_itow_s(flat+perlvl*(*playerLevel), istr, 10);
		if(showLabel)
			font->DrawText(NULL, labelName.c_str(), -1, &labelCoords, DT_NOCLIP, labelFontColor);
		font->DrawText(NULL, istr, -1, &indiCoords, DT_NOCLIP, indiFontColor);

		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName.c_str(), L"draw is done");
	}
	
	void PrepareRender(PDIRECT3DDEVICE9 pDevice){
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName.c_str(), L"preparing render");

		//Create DirectX objects
		D3DXCreateFont(pDevice, indiFontSize, 0, indiFontWeight, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, indiFontName.c_str(), &font);
		
		//Find needed pointers
		TryToInitPointers();

		LOG_VERBOSE_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName.c_str(), L"is ready to draw");
	}

private:
	//Get pointers we can grab only in runtime, after some special in-game events
	void TryToInitPointers(){
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName.c_str(), L"trying to find needed pointers");
		
		DWORD res = *((DWORD *)LOL_MEM_PLAYER_LEVEL_PTR);
		if(!res)
			return;
		res += LOL_MEM_PLAYER_LEVEL_OFFSET1;
		res = *((DWORD *)res);
		if(!res)
			return;
		res += LOL_MEM_PLAYER_LEVEL_OFFSET2;
		playerLevel = (int*)res;
		
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName.c_str(), L"all pointers are found");
	}

	//In-game pointers
	static int *playerLevel;
	bool *isAlivePtr;

	//Name for config and logging
	wstring innerName;

	int flat;
	int perlvl; //additional damage

	/************************************************************************/
	/*    OVERLAY SETTINGS                                                  */
	/************************************************************************/
	//Label
	wstring labelName;
	bool showLabel;
	RECT labelCoords;
	DWORD labelFontColor;
	
	//Timer
	RECT indiCoords;
	int indiFontSize;
	int indiFontWeight;
	wstring indiFontName;
	DWORD indiFontColor;

	//Overlay fonts
	LPD3DXFONT font;
};

//Init static variables.
// !!! We can init static variable in class ONLY if it's a const variable!
//     So we need to init it outside of the class definition
int *JungleTime::SmiteIgniteIndicator::playerLevel = NULL;
}
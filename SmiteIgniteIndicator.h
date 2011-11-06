#pragma once

#include "defines.h"

#include "IRenderable.h"

#define TYPE_SMITE 1
#define TYPE_IGNITE 2

namespace JungleTime{

class SmiteIgniteIndicator : public JungleTime::IRenderableObject
{
public:
	SmiteIgniteIndicator(LPCWSTR innerName, int flat, int perlvl)
		: innerName(innerName), flat(flat), perlvl(perlvl), font(0), resourcesAreReady(false){
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"loading...");

		/************************************************************************/
		/* OVERLAY                                                              */
		/************************************************************************/
		//Label
		labelName = INIReadStr(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_label_name"), CONFIG_NAME_DESIGN);
		showLabel = !!INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_show_label"), CONFIG_NAME_DESIGN);
		labelFontColor = INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_label_font_color"), CONFIG_NAME_DESIGN);
		SetRect(&labelCoords,
			INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_label_pos_x"), CONFIG_NAME_DESIGN),
			INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_label_pos_y"), CONFIG_NAME_DESIGN),
			300, 300);

		//Indicator
		indiFontSize = INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_indi_font_size"), CONFIG_NAME_DESIGN);
		indiFontWeight = INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_indi_font_weight"), CONFIG_NAME_DESIGN);
		indiFontName = INIReadStr(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_indi_font_name"), CONFIG_NAME_DESIGN);
		indiFontColor = INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_indi_font_color"), CONFIG_NAME_DESIGN);

		SetRect(&indiCoords,
			INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_indi_pos_x"), CONFIG_NAME_DESIGN),
			INIReadInt(L"overlay", INI_NAME_WITH_PREFIX(innerName, L"_indi_pos_y"), CONFIG_NAME_DESIGN),
			300, 300);
		
		LOG_VERBOSE_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"loaded");
	}

	virtual ~SmiteIgniteIndicator(void){
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"unloading...");	
		
		//Delete used mem
		delete labelName;
		delete indiFontName;
				
		//Release resources only if application loop is alive, we dont need to freeze application in the process list
		if(font != NULL && !IS_DX_LOOP_DEAD)
			font->Release();
		resourcesAreReady = true;
		LOG_VERBOSE_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"unloaded");	
	}

	void Render(PDIRECT3DDEVICE9 pDevice, int frameNum, int curTimeSecs){
		if(!resourcesAreReady){
			LOG_WARNING_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"resources was not ready in the real application way, trying to create it by myself...");
			PrepareResources(pDevice);
			if(!resourcesAreReady){
				LOG_ERROR_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"but i cant create resources, waiting 500 ms");
				Sleep(500);
				return;
			}
		}
		LOG_DEBUG_EF_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"drawing");

		if(!playerLevel && frameNum%30 == 0){
			TryToInitPointers();

			//But if still not available...
			#ifdef _DEBUG
			if(!playerLevel)
				LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"pointers still not available");
			#endif
		}

		//If we still cannot find needed pointers - we have nothing to render...
		if(!playerLevel){
			LOG_DEBUG_EF_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"pointers still not available");
			return;
		}
		
		wchar_t istr[30];
		_itow_s(flat+perlvl*(*playerLevel), istr, 10);
		if(showLabel)
			font->DrawText(NULL, labelName, -1, &labelCoords, DT_NOCLIP, labelFontColor);
		font->DrawText(NULL, istr, -1, &indiCoords, DT_NOCLIP, indiFontColor);
		resourcesAreReady = false;
		LOG_DEBUG_EF_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"draw is done");
	}
	
	void Init(PDIRECT3DDEVICE9 pDevice){
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"preparing...");
		//Find needed pointers
		TryToInitPointers();
		LOG_VERBOSE_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"is ready");
	}

	void OnLostDevice(){
		if(font)
			font->OnLostDevice();
	}

	void OnResetDevice(PDIRECT3DDEVICE9 pDevice){
		if(font)
			font->OnResetDevice();
	}

	void PrepareResources(PDIRECT3DDEVICE9 pDevice){
		//Create DirectX objects
		HRESULT res = D3DXCreateFont(pDevice, indiFontSize, 0, indiFontWeight, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, indiFontName, &font);
		if(res != S_OK){
			LOG_ERROR_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"cannot create resources");
			return;
		}
		resourcesAreReady = true;
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"resources are ready");
	}

private:
	//Get pointers we can grab only in runtime, after some special in-game events
	void TryToInitPointers(){
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"trying to find needed pointers");
		
		DWORD res = *((DWORD *)LOL_MEM_PLAYER_LEVEL_PTR);
		if(!res)
			return;
		res += LOL_MEM_PLAYER_LEVEL_OFFSET1;
		res = *((DWORD *)res);
		if(!res)
			return;
		res += LOL_MEM_PLAYER_LEVEL_OFFSET2;
		playerLevel = (int*)res;
		
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", L"ObjectIndicators", innerName, L"all pointers are found");
	}

	//In-game pointers
	static int *playerLevel;
	bool *isAlivePtr;

	//Name for config and logging
	LPCWSTR innerName;

	int flat;
	int perlvl; //additional damage

	/************************************************************************/
	/*    OVERLAY SETTINGS                                                  */
	/************************************************************************/
	//Label
	LPCWSTR labelName;
	bool showLabel;
	RECT labelCoords;
	DWORD labelFontColor;
	
	//Timer
	RECT indiCoords;
	int indiFontSize;
	int indiFontWeight;
	LPCWSTR indiFontName;
	DWORD indiFontColor;

	//Overlay fonts
	LPD3DXFONT font;
	bool resourcesAreReady;
};

//Init static variables.
// !!! We can init static variable in class ONLY if it's a const variable!
//     So we need to init it outside of the class definition
int *JungleTime::SmiteIgniteIndicator::playerLevel = NULL;
}
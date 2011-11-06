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
		: innerName(innerName), flat(flat), perlvl(perlvl), font(0){
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"loading...");

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
		
		LOG_VERBOSE_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"loaded");
	}

	virtual ~SmiteIgniteIndicator(void){
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"unloading...");	
		
		//Delete used mem
		delete labelName;
		delete indiFontName;
				
		//Release resources only if application loop is alive, we dont need to freeze application in the process list
		if(font != NULL && !IS_DX_LOOP_DEAD)
			font->Release();
		LOG_VERBOSE_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"unloaded");	
	}

	void Render(PDIRECT3DDEVICE9 pDevice, int frameNum, int curTimeSecs){
		LOG_DEBUG_EF_MF(L"SmiteIgniteIndicator.h", curTimeSecs, L"ObjectIndicators", innerName, L"drawing");

		//!playerLevel - if not found in the initiation
		//(frameNum%100 == 0) - dont spam
		//(curTimeSecs > 10) - if not found in the initiatin, we dont need to search for it before the game starts
		if(!playerLevel && (frameNum%100 == 0) && (curTimeSecs > 3)){
			TryToInitPointers();

			//But if still not available...
			if(!playerLevel && curTimeSecs > 10)
				LOG_ERROR_MF(L"SmiteIgniteIndicator.h", curTimeSecs, L"ObjectIndicators", innerName, L"player level pointer is broken! Not available in 10 seconds after the spawn time.");
		}

		//If we still cannot find needed pointers - we have nothing to render...
		if(!playerLevel){
			return;
		}
		
		int errCode = 0, level = 0;
		if((errCode = TryDirectReadMem<int>(&level, playerLevel)) != (int)&level){
			LOG_ERROR_CODE_MF(L"SmiteIgniteIndicator.h", curTimeSecs, L"ObjectIndicators", innerName, L"cannot read player level memory, error code: ", errCode);
			return;
		}
		
		wchar_t istr[30];
		_itow_s(flat+perlvl*(level), istr, 10);
		if(showLabel)
			font->DrawText(NULL, labelName, -1, &labelCoords, DT_NOCLIP, labelFontColor);
		font->DrawText(NULL, istr, -1, &indiCoords, DT_NOCLIP, indiFontColor);
		LOG_DEBUG_EF_MF(L"SmiteIgniteIndicator.h", curTimeSecs, L"ObjectIndicators", innerName, L"draw is done");
	}
	
	bool Init(PDIRECT3DDEVICE9 pDevice){
		bool res = true;
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"preparing...");
		//Find needed pointers
		/*res = res && *///TryToInitPointers(); //We dont need to load it before the game starts
		LOG_VERBOSE_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"is ready");
		return res;
	}

	void OnLostDevice(){
		if(font)
			font->OnLostDevice();
	}

	void OnResetDevice(PDIRECT3DDEVICE9 pDevice){
		if(font)
			font->OnResetDevice();
	}

	bool PrepareResources(PDIRECT3DDEVICE9 pDevice){
		//Create DirectX objects
		HRESULT res = D3DXCreateFont(pDevice, indiFontSize, 0, indiFontWeight, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, indiFontName, &font);
		if(res != S_OK){
			LOG_ERROR_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"cannot create resources");
			return false;
		}
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"resources are ready");
		return true;
	}

private:
	//Get pointers we can grab only in runtime, after some special in-game events
	bool TryToInitPointers(){
		LOG_DEBUG_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"trying to find needed pointers");
		
		int errCode = 0;
		DWORD tmp = NULL, tmp2 = NULL;//for teh logging

		//LOL_MEM_PLAYER_LEVEL_PTR->
		if((errCode = TryCopyMem(&tmp, (void*)LOL_MEM_PLAYER_LEVEL_PTR, sizeof(void*))) != (int)&tmp){
			LOG_ERROR_CODE_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"cannot read player struct memory, error code: ", errCode);
			return false;
		}
		//==NULL
		if(tmp == 0x0){
			LOG_ERROR_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"cannot read player struct memory: struct pointer is null");
			return false;
		}

		//LOL_MEM_PLAYER_LEVEL_PTR->x+LOL_MEM_PLAYER_LEVEL_OFFSET1
		if((errCode = TryCopyMem(&tmp2, (void*)(tmp+LOL_MEM_PLAYER_LEVEL_OFFSET1), sizeof(void*))) != (int)&tmp2){
			LOG_ERROR_CODE_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"cannot read player struct with OFFSET1 memory, error code: ", errCode);
			return false;
		}
		//==NULL
		if(tmp2 == 0x0){
			LOG_ERROR_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"cannot read player struct with OFFSET1 memory: struct pointer is null");
			return false;
		}

		//LOL_MEM_PLAYER_LEVEL_PTR->tmp+LOL_MEM_PLAYER_LEVEL_OFFSET1->tmp2+LOL_MEM_PLAYER_LEVEL_OFFSET2
		playerLevel = (int*)(tmp2+LOL_MEM_PLAYER_LEVEL_OFFSET2);

		wchar_t buf[2048];
		swprintf(buf, 2048, L"Current level address: %p->%p+%p->%p+%p->%p = int", LOL_MEM_PLAYER_LEVEL_PTR, tmp, LOL_MEM_PLAYER_LEVEL_OFFSET1, tmp2, LOL_MEM_PLAYER_LEVEL_OFFSET2, playerLevel);
		LOG_VERBOSE_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, buf);

		//Check value
		int tmp3 = 0;
		if((errCode = TryCopyMem(&tmp3, (void*)playerLevel, sizeof(int))) != (int)&tmp3){
			LOG_ERROR_CODE_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"cannot check player level memory, error code: ", errCode);
			return false;
		}

		if(tmp3 < 0 || tmp3 > 18)
			LOG_ERROR_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"PlayerLevel pointer is broken! <0 or >18");

		LOG_VERBOSE_MF(L"SmiteIgniteIndicator.h", 0, L"ObjectIndicators", innerName, L"all pointers are found");
		return true;
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
};

//Init static variables.
// !!! We can init static variable in class ONLY if it's a const variable!
//     So we need to init it outside of the class definition
int *JungleTime::SmiteIgniteIndicator::playerLevel = NULL;
}
#pragma once

#include "defines.h"

namespace JungleTime{

class IRenderableObject
{
public:
	virtual ~IRenderableObject(void){};

	virtual void Render(PDIRECT3DDEVICE9 pDevice, int frameNum, int curTimeSecs) = 0;
	virtual void Init(PDIRECT3DDEVICE9 pDevice) = 0;
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice(PDIRECT3DDEVICE9 pDevice) = 0;
	virtual void PrepareResources(PDIRECT3DDEVICE9 pDevice) = 0;
};

}
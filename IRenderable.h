#pragma once

#include "defines.h"

namespace JungleTime{

class IRenderableObject
{
public:
	virtual ~IRenderableObject(void){};

	virtual void Render(PDIRECT3DDEVICE9 pDevice, int frameNum, int curTimeSecs) = 0;
	virtual void PrepareRender(PDIRECT3DDEVICE9 pDevice) = 0;
};

}
#pragma once

#include "defines.h"

#define OBJECT_TIMER_PATTERN_SIZE 4

namespace JungleTime{

class ObjectTimer :
	public boost::timer
{
public:
	ObjectTimer(string innerName, string labelName, double cooldown);
	virtual ~ObjectTimer(void);

	string innerName;
	string labelName;
	double cooldown;
	COLORREF patternLight[OBJECT_TIMER_PATTERN_SIZE*OBJECT_TIMER_PATTERN_SIZE];
	COLORREF patternShadow[OBJECT_TIMER_PATTERN_SIZE*OBJECT_TIMER_PATTERN_SIZE];
	COLORREF patternAvail[OBJECT_TIMER_PATTERN_SIZE*OBJECT_TIMER_PATTERN_SIZE];
};

}
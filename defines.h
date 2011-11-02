#pragma once

#define WIN32_LEAN_AND_MEAN
//General things
#include <windows.h>
#include <process.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <stdio.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <functional>

//DirectX
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

using namespace std;

//Current timers
#include <time.h>

#include <iostream>

#include "D3D9Proxy/common.h"

#include "log.h"

LOG_USE;

#define LOL_WINDOW_NAME L"League of Legends (TM) Client"
#define IS_DX_LOOP_DEAD (!FindWindow(NULL, LOL_WINDOW_NAME))

/************************************************************************/
/*    OFFSETS AND DEFINES FOR MAP CHECKING                              */
/************************************************************************/
#define LOL_MEM_MAP_NAME_OFFSET 0x02BF2B3C
#define LOL_MAP_SUMMONERS_RIFT_NAME "Map1"
#define LOL_MAP_SUMMONERS_RIFT_AUT_NAME "Map2"

#define LOL_MAP_SUMMONERS_RIFT 1

/************************************************************************/
/*    NET-OBJECTS OFFSETS AND POINTERS                                  */
/************************************************************************/
#define LOL_MEM_NETOBJECT_PATTERN_OFFSET 0x0000005C
#define LOL_MEM_NETOBJECT_IS_ALIVE_OFFSET 0x00000134
#define LOL_MEM_NETOBJECTS_ARRAY_PTR 0x02C8022C
#define LOL_MEM_NETOBJECTS_MAX_PTR_INT 0x02C81220
#define LOL_MEM_NETOBJECTS_MAX 2500 //anyway...

/************************************************************************/
/*    NET-OBJECTS MEMORY PATTERNS                                       */
/************************************************************************/
#define LOL_MEM_NETOBJECT_TOP_WOLVES_PATTERN 0x46234E18 //18 4E 23 46
#define LOL_MEM_NETOBJECT_TOP_GOLEMS_PATTERN 0x45B593B6 //B6 93 B5 45
#define LOL_MEM_NETOBJECT_TOP_WRAITHS_PATTERN 0x45E72AF2 //F2 2A E7 45
#define LOL_MEM_NETOBJECT_TOP_RED_PATTERN 0x45CB41ED //ED 41 CB 45
#define LOL_MEM_NETOBJECT_TOP_BLUE_PATTERN 0x46224A6C //6C 4A 22 46

#define LOL_MEM_NETOBJECT_BOT_WOLVES_PATTERN 0x454FBADA //DA BA 4F 45
#define LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN 0x45F766BD //BD 66 F7 45
#define LOL_MEM_NETOBJECT_BOT_WRAITHS_PATTERN 0x45C9A752 //52 A7 C9 45
#define LOL_MEM_NETOBJECT_BOT_RED_PATTERN 0x45E8FCEC //EC FC E8 45
#define LOL_MEM_NETOBJECT_BOT_BLUE_PATTERN 0x45630B34 //34 0B 63 45

#define LOL_MEM_NETOBJECT_DRAGON_PATTERN 0x4613CE14 //14 CE 13 46
#define LOL_MEM_NETOBJECT_BARON_PATTERN 0x458FC3F6 //F6 C3 8F 45

/************************************************************************/
/*    IN-GAME TIME HELPERS AND POINTERS/OFFSETS                         */
/************************************************************************/
#define LOL_MEM_START_TIME_PTR 0x01BA6088
#define LOL_MEM_CURRENT_TIME_STRUCT_PTR 0x00B46858
#define LOL_MEM_CURRENT_TIME_OFFSET 0x00000008
#define LOL_MEM_GET_GAMETIME() ((ingameClockNow && ingameClockStart)? ((int)floor(*ingameClockNow - *ingameClockStart)) : 0)

#define LOL_MEM_PLAYER_LEVEL_PTR 0x00B46974
#define LOL_MEM_PLAYER_LEVEL_OFFSET1 0x000000DC
#define LOL_MEM_PLAYER_LEVEL_OFFSET2 0x00000100

/************************************************************************/
/*     CONFIG FILE NAMES                                                */
/************************************************************************/
#define CONFIG_NAME_DESIGN L"./jt2_design.ini" //Design of overlay
#define CONFIG_NAME_USER L"./jt2_user.ini" //User configuration file

#define INI_NAME_WITH_PREFIX(prefix, postfix) ((wstring(##prefix) + ##postfix).c_str())
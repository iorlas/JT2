#pragma once

#define WIN32_LEAN_AND_MEAN
//General things
#include <windows.h>
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

#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>

#include "D3D9Proxy/common.h"

#include "log.h"

LOG_USE;

#define LOL_WINDOW_NAME L"League of Legends (TM) Client"

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
#define LOL_MEM_NETOBJECT_BOT_WOLVES_PATTERN 0x454FBADA //DA BA 4F 45
#define LOL_MEM_NETOBJECT_BOT_GOLEMS_PATTERN 0x45F766BD //BD 66 F7 45


/************************************************************************/
/*    IN-GAME TIME HELPERS AND POINTERS/OFFSETS                         */
/************************************************************************/
#define LOL_MEM_START_TIME_PTR 0x01BA1548
#define LOL_MEM_CURRENT_TIME_STRUCT_PTR 0x00B42224
#define LOL_MEM_CURRENT_TIME_OFFSET 0x00000008
#define LOL_MEM_GET_GAMETIME() ((ingameClockNow && ingameClockStart)? ((int)floor(*ingameClockNow - *ingameClockStart)) : 0)
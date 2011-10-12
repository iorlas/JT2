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

#include "D3D9Proxy/common.h"

#include "log.h"

LOG_USE;
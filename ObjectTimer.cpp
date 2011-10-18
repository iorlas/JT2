#include "ObjectTimer.h"

bool CheckPixelMatrix(HDC windowDC, int *coords, COLORREF *patterns){
	COLORREF cur;
	for (int i = 0; i < OBJECT_TIMER_PATTERN_SIZE*OBJECT_TIMER_PATTERN_SIZE; i++){
		cur = GetPixel(windowDC, coords[0]+(i%OBJECT_TIMER_PATTERN_SIZE), coords[1]+(i/OBJECT_TIMER_PATTERN_SIZE));
		if(cur != patterns[i]){
			return false;
		}
	}
	return true;
}


bool ParsePatternLine(string line, COLORREF *patternOut){
	using namespace boost;

	//Split into tokens by "|"
	vector<string> pixelColors;
	split(pixelColors, line, is_any_of("|"), token_compress_on);
	if (pixelColors.size() != OBJECT_TIMER_PATTERN_SIZE*OBJECT_TIMER_PATTERN_SIZE)
		return false;
	//Parse numbers
	for(int i = 0; i != pixelColors.size(); i++){
		std::stringstream s(pixelColors.at(i));
		s >> patternOut[i]; 
	}
	return true;
}

wstring StringToWString(const string& s){
	wstring temp(s.length(),L' ');
	copy(s.begin(), s.end(), temp.begin());
	return temp; 
}

namespace JungleTime{

ObjectTimer::ObjectTimer(wstring innerName, double cooldown, boost::property_tree::ptree config)
	: mainTimer(), innerName(innerName), cooldown(cooldown), isAlive(false){
	LOG_VERBOSE((L"Object Timers: Loading " + innerName).c_str());

	//Damn, i hate it, but we need a std::string, not a wstring to deal with ptree
	string innerNameStr(innerName.begin(), innerName.end());

	
	/************************************************************************/
	/*  PATTERNS                                                            */
	/************************************************************************/
	//Loading pattern: light
	string patternUnparsed = config.get<string>("patterns."+innerNameStr+"_light");
	if(!ParsePatternLine(patternUnparsed, patternLight)){
		LOG_VERBOSE((L"Object Timers: Cannot load " + innerName + L", light pattern is broken!").c_str());
		return;
	}
	//Loading pattern: shadow
	patternUnparsed = config.get<string>("patterns."+innerNameStr+"_shadow");
	if(!ParsePatternLine(patternUnparsed, patternShadow)){
		LOG_VERBOSE((L"Object Timers: Cannot load " + innerName + L", shadow pattern is broken!").c_str());
		return;
	}
	//Loading pattern: avail
	patternUnparsed = config.get<string>("patterns."+innerNameStr);
	if(!ParsePatternLine(patternUnparsed, patternAvail)){
		LOG_VERBOSE((L"Object Timers: Cannot load " + innerName + L", avail pattern is broken!").c_str());
		return;
	}
	//Coordinates of the left top pixel
	coords[0] = config.get<int>("patterns."+innerNameStr+"_pos_x");
	coords[1] = config.get<int>("patterns."+innerNameStr+"_pos_y");

	
	/************************************************************************/
	/* OVERLAY                                                              */
	/************************************************************************/
	//Label
	labelName = StringToWString(config.get<string>("overlay."+innerNameStr+"_label_name"));
	showLabel = config.get<bool>("overlay."+innerNameStr+"_show_label");
	SetRect(&labelCoords,
		config.get<int>("overlay."+innerNameStr+"_label_pos_x"),
		config.get<int>("overlay."+innerNameStr+"_label_pos_y"),
		300, 300);

	//Timer
	timerFontSize = config.get<int>("overlay."+innerNameStr+"_timer_font_size");
	timerFontWeight = config.get<int>("overlay."+innerNameStr+"_timer_font_weight");
	timerFontName = StringToWString(config.get<string>("overlay."+innerNameStr+"_timer_font_name"));
	timerFontColor = config.get<DWORD>("overlay."+innerNameStr+"_timer_font_color");
	timerFontRedlineColor = config.get<DWORD>("overlay."+innerNameStr+"_timer_font_redline_color");
	timerFontAliveColor = config.get<DWORD>("overlay."+innerNameStr+"_timer_font_alive_color");

	redLine = config.get<double>("overlay."+innerNameStr+"_red_line");
	SetRect(&timerCoords,
		config.get<int>("overlay."+innerNameStr+"_timer_pos_x"),
		config.get<int>("overlay."+innerNameStr+"_timer_pos_y"),
		300, 300);

	//Announce
	showAnnounce = config.get<bool>("overlay."+innerNameStr+"_show_announce");
	announceFontSize = config.get<int>("overlay."+innerNameStr+"_announce_font_size");
	announceFontWeight = config.get<int>("overlay."+innerNameStr+"_announce_font_weight");
	announceFontName = StringToWString(config.get<string>("overlay."+innerNameStr+"_announce_font_name"));
	announceFontColor = config.get<DWORD>("overlay."+innerNameStr+"_announce_font_color");
	SetRect(&announceCoords,
		config.get<int>("overlay."+innerNameStr+"_announce_pos_x"),
		config.get<int>("overlay."+innerNameStr+"_announce_pos_y"),
		300, 300);

	LOG_VERBOSE((L"Object Timers: Loading " + innerName + L" - done").c_str());
}

void ObjectTimer::Render(PDIRECT3DDEVICE9 pDevice){
	//Show label only if we need
	if(showLabel)
		//0xFFFFFFFF
		timerFont->DrawText(NULL, labelName.c_str(), -1, &labelCoords, DT_NOCLIP, timerFontColor);
	if(isAlive)
		//0xFFB5E61D
		timerFont->DrawText(NULL, TIMER_LABEL_ALIVE, -1, &timerCoords, DT_NOCLIP, timerFontAliveColor);
	else{
		boost::mutex::scoped_lock l(timerMutex);
		double timeLeft = cooldown - mainTimer.elapsed();
		
		//I don't want to show you a negative numbers
		timeLeft = timeLeft < 0 ? 0 : timeLeft;

		//Fast convert to wstring
		wchar_t istr[32];
		_itow_s(timeLeft, istr, 10);

		//Highlight if we have small time to get it alive
		if(timeLeft <= redLine)
			//0xFFFFC90E
			timerFont->DrawText(NULL, istr, -1, &timerCoords, DT_NOCLIP, timerFontRedlineColor);
		else
			//0xFFFFFFFF
			timerFont->DrawText(NULL, istr, -1, &timerCoords, DT_NOCLIP, timerFontColor);
	}
}

void ObjectTimer::CheckPixels(HDC windowDC){
	if(!isAlive && CheckPixelMatrix(windowDC, coords, patternAvail))
			Stop();
	else if(isAlive && CheckPixelMatrix(windowDC, coords, patternShadow))
			Start();
	else if(isAlive && CheckPixelMatrix(windowDC, coords, patternLight))
			Start();
}

void ObjectTimer::PrepareRender(PDIRECT3DDEVICE9 pDevice){
	D3DXCreateFont(pDevice, timerFontSize, 0, timerFontWeight, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, timerFontName.c_str(), &timerFont);
	Start();
}

void ObjectTimer::Start(){
	boost::mutex::scoped_lock l(timerMutex);
	isAlive = false;
	mainTimer.restart();
	LOG_VERBOSE((L"Object Timers: " + innerName + L" - START").c_str());
}

void ObjectTimer::Stop(){
	isAlive = true;
	LOG_VERBOSE((L"Object Timers: " + innerName + L" - STOP").c_str());
}

ObjectTimer::~ObjectTimer(void){
	//TODO: Release temp resources
	//BUG: If application closes, we cant release any resource. Perhaps, because of loop, it never runs after end of app.
	//timerFont->Release();
}

}
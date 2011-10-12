#pragma once

#include "DX9Manager.h"

#include "ObjectTimer.h"

namespace JungleTime{

class Core : public DX9Manager{
public:
	Core(void);
	~Core(void);
	
	int Init(void);	

	static Core* singleton;
private:
	void OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice);
	void OnDXInitiated(void);
	void OnDXEndScene(LPDIRECT3DDEVICE9 pDevice);

	vector<boost::shared_ptr<ObjectTimer>> timers;

	boost::property_tree::ptree userConfig;
	boost::property_tree::ptree resConfig;
};

}

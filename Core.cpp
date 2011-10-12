#include "defines.h"
#include "Core.h"

namespace JungleTime{

Core* Core::singleton;

Core::Core(void) : DX9Manager(){
	LOG_VERBOSE(L"* Core created");
}

Core::~Core(void){
	LOG_VERBOSE(L"* Core closed");
}

int Core::Init(void){
	LOG_VERBOSE(L"* Core initiated");
	return TRUE;
}

void Core::OnDXFirstFrame(LPDIRECT3DDEVICE9 pDevice){

}
void Core::OnDXInitiated(void){

}
void Core::OnDXEndScene(LPDIRECT3DDEVICE9 pDevice){

}

}
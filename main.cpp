#include "defines.h"

#include "MemoryPatcher.h"
#include "renderer.hpp"
#include "ui.hpp"
#include "config.hpp"
#include "lua.hpp"
#include "rawinput.hpp"
#include "console.hpp"
#include "menu.hpp"


#pragma pack(1)
HINSTANCE hlThis = 0;
HINSTANCE hlD3D9 = 0;
FARPROC origProc[15] = {0};

IDirect3DStateBlock9* pStateBlock = NULL;
IDirect3DStateBlock9* pStateBlockOrig = NULL;

LONG OldWndProc;


LRESULT CALLBACK NewWndProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

	
	return CallWindowProc((WNDPROC)OldWndProc, Hwnd, Message, wParam, lParam);
}


void OnDeviceCreate(IDirect3DDevice9* device) {
	renderer::init(device);
	console::init();
	menu::init();

	device->CreateStateBlock(D3DSBT_ALL, &pStateBlock);

	D3DDEVICE_CREATION_PARAMETERS params;
	device->GetCreationParameters(&params);

	OldWndProc = SetWindowLong(params.hFocusWindow, GWL_WNDPROC, (long)NewWndProc);
}

void OnDevicePreReset() {
	
	if (pStateBlock) {
		pStateBlock->Release();
		pStateBlock = NULL;
	}
	if (pStateBlockOrig) {
		pStateBlockOrig->Release();
		pStateBlockOrig = NULL;
	}
}

void OnDevicePostReset(IDirect3DDevice9* device, HRESULT result) {
	if (SUCCEEDED(result)) {
		
		device->CreateStateBlock(D3DSBT_ALL, &pStateBlock);
	}
}

void OnDeviceEndScene(IDirect3DDevice9* device) {
	
	HRESULT coopResult = device->TestCooperativeLevel();
	if (coopResult == D3DERR_DEVICELOST || coopResult == D3DERR_DRIVERINTERNALERROR) {
		return; 
	}
	
	
	if (pStateBlock) {
		device->CreateStateBlock(D3DSBT_ALL, &pStateBlockOrig);
		if (pStateBlockOrig && SUCCEEDED(pStateBlockOrig)) {
			pStateBlock->Apply();

			renderer::render();

			pStateBlockOrig->Apply();
			pStateBlockOrig->Release();
			pStateBlockOrig = NULL;
		}
	}
}




BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID){
	if (reason == DLL_PROCESS_ATTACH){


		lua::hook();
		rawinput::hook();
		config::init();
		
		
		
		if (config::external_console)
		{
			AllocConsole();
			freopen("CONOUT$", "w", stdout);
			freopen("CONIN$", "r", stdin);
		}
		hlThis = hInst;

		
		char infoBuf[MAX_PATH];
		GetSystemDirectory(infoBuf, MAX_PATH);
		strcat_s(infoBuf, MAX_PATH, "\\d3d9.dll");

		
		hlD3D9 = LoadLibrary(infoBuf);
		if (!hlD3D9){
			MessageBox(NULL, "D3D9 Proxy DLL error", "Cannot find original d3d9.dll in the system directory!", MB_OK | MB_ICONERROR);
			return FALSE;
		}

		
		origProc[0] = GetProcAddress(hlD3D9, "D3DPERF_BeginEvent");
		origProc[1] = GetProcAddress(hlD3D9, "D3DPERF_EndEvent");
		origProc[2] = GetProcAddress(hlD3D9, "D3DPERF_GetStatus");
		origProc[3] = GetProcAddress(hlD3D9, "D3DPERF_QueryRepeatFrame");
		origProc[4] = GetProcAddress(hlD3D9, "D3DPERF_SetMarker");
		origProc[5] = GetProcAddress(hlD3D9, "D3DPERF_SetOptions");
		origProc[6] = GetProcAddress(hlD3D9, "D3DPERF_SetRegion");
		origProc[7] = GetProcAddress(hlD3D9, "DebugSetLevel");
		origProc[8] = GetProcAddress(hlD3D9, "DebugSetMute");
		origProc[9] = GetProcAddress(hlD3D9, "Direct3DCreate9");
		origProc[10] = GetProcAddress(hlD3D9, "Direct3DCreate9Ex");
		origProc[11] = GetProcAddress(hlD3D9, "Direct3DShaderValidatorCreate9");
		origProc[12] = GetProcAddress(hlD3D9, "PSGPError");
		origProc[13] = GetProcAddress(hlD3D9, "PSGPSampleTexture");

		IDirect3DDevice9Proxy::callbacks[CREATE] = OnDeviceCreate;
		IDirect3DDevice9Proxy::callbacks[ENDSCENE] = OnDeviceEndScene;
		IDirect3DDevice9Proxy::callbacks[PRERESET] = OnDevicePreReset;
		IDirect3DDevice9Proxy::callbacks[POSTRESET] = OnDevicePostReset;
	}else if (reason == DLL_PROCESS_DETACH){
		
		if (pStateBlock) {
			pStateBlock->Release();
			pStateBlock = NULL;
		}
		if (pStateBlockOrig) {
			pStateBlockOrig->Release();
			pStateBlockOrig = NULL;
		}
		FreeLibrary(hlD3D9);
	}
	return TRUE;
}


extern "C" IDirect3D9* WINAPI __ProxyFunc9(UINT SDKVersion)
{

	
	typedef IDirect3D9* (WINAPI* Direct3DCreate9Func)(UINT sdkver);
	Direct3DCreate9Func origDirect3DCreate9 = (Direct3DCreate9Func)GetProcAddress(hlD3D9, "Direct3DCreate9");
	IDirect3D9* res = origDirect3DCreate9(SDKVersion);
	return new IDirect3D9Proxy(res);
}

extern "C" __declspec(naked) void __stdcall __ProxyFunc10(){
	__asm{
		jmp origProc[10*4];
	}
}

extern "C" __declspec(naked) void __stdcall __ProxyFunc0(){
	__asm{
		jmp origProc[0*4];
	}
}
extern "C" __declspec(naked) void __stdcall __ProxyFunc1(){
	__asm{
		jmp origProc[1*4];
	}
}
extern "C" __declspec(naked) void __stdcall __ProxyFunc2(){
	__asm{
		jmp origProc[2*4];
	}
}
extern "C" __declspec(naked) void __stdcall __ProxyFunc3(){
	__asm{
		jmp origProc[3*4];
	}
}
extern "C" __declspec(naked) void __stdcall __ProxyFunc4(){
	__asm{
		jmp origProc[4*4];
	}
}
extern "C" __declspec(naked) void __stdcall __ProxyFunc5(){
	__asm{
		jmp origProc[5*4];
	}
}
extern "C" __declspec(naked) void __stdcall __ProxyFunc6(){
	__asm{
		jmp origProc[6*4];
	}
}
extern "C" __declspec(naked) void __stdcall __ProxyFunc7(){
	__asm{
		jmp origProc[7*4];
	}
}
extern "C" __declspec(naked) void __stdcall __ProxyFunc8(){
	__asm{
		jmp origProc[8*4];
	}
}
extern "C" __declspec(naked) void __stdcall __ProxyFunc11(){
	__asm{
		jmp origProc[11*4];
	}
}
extern "C" __declspec(naked) void __stdcall __ProxyFunc12(){
	__asm{
		jmp origProc[12*4];
	}
}
extern "C" __declspec(naked) void __stdcall __ProxyFunc13(){
	__asm{
		jmp origProc[13*4];
	}
}
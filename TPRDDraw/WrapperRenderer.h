#include "stdafx.h"
#include "IRenderer.h"

class WrapperSurface;
class WrapperPalette;

enum DisplayMode {
	UNASSIGNED,
	FULLSCREEN,
	WINDOWED
};

class WrapperRenderer : IRenderer {
public:
	/*** IUnknown methods ***/
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	/*** IDirectDraw methods ***/
	virtual HRESULT __stdcall CreatePalette(DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, IUnknown FAR*);
	virtual HRESULT __stdcall CreateSurface(LPDDSURFACEDESC, LPDIRECTDRAWSURFACE FAR*, IUnknown FAR*);
	virtual HRESULT __stdcall EnumDisplayModes(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK);
	virtual HRESULT __stdcall SetCooperativeLevel(HWND, DWORD);
	virtual HRESULT __stdcall SetDisplayMode(DWORD, DWORD, DWORD);
	virtual HRESULT __stdcall WaitForVerticalBlank(DWORD, HANDLE);

	WrapperRenderer();

	DWORD refCount;
	HWND windowHandle;
	IDirect3D9* d3d;
	IDirect3DDevice9* d3ddev;
	DWORD width;
	DWORD height;
	WrapperPalette* palette;
	WrapperSurface* surface;
	DisplayMode previousDisplayMode;
	RECT storedWinPos;
};

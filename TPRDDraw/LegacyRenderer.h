#include "stdafx.h"
#include "IRenderer.h"

class LegacySurface;
class WrapperPalette;

//This surface is only used when in windowed mode

class LegacyRenderer : IRenderer {
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

	HRESULT FallbackToFullscreen(LPDDSURFACEDESC, LPDIRECTDRAWSURFACE FAR*);
	static HRESULT __stdcall EnumModesCallback(LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext);

	LegacyRenderer();

	LPDIRECTDRAW directDraw;
	DWORD refCount;
	HWND windowHandle;
	DWORD width;
	DWORD height;
	LegacySurface* primarySurface;
	WrapperPalette* palette;
	LONG cooperativeLevel;
	RECT storedWinPos;
	INT desktopBitCount;
};

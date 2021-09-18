#pragma once
#include "stdafx.h"

class IRenderer : public IDirectDraw {
public:
	/*** IUnknown methods ***/
	virtual HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	/*** IDirectDraw methods ***/
	virtual HRESULT __stdcall Compact();
	virtual HRESULT __stdcall CreateClipper(DWORD, LPDIRECTDRAWCLIPPER*, IUnknown*);
	virtual HRESULT __stdcall CreatePalette(DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE*, IUnknown*);
	virtual HRESULT __stdcall CreateSurface(LPDDSURFACEDESC, LPDIRECTDRAWSURFACE*, IUnknown*);
	virtual HRESULT __stdcall DuplicateSurface(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE*);
	virtual HRESULT __stdcall EnumDisplayModes(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK);
	virtual HRESULT __stdcall EnumSurfaces(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK);
	virtual HRESULT __stdcall FlipToGDISurface();
	virtual HRESULT __stdcall GetCaps(LPDDCAPS, LPDDCAPS);
	virtual HRESULT __stdcall GetDisplayMode(LPDDSURFACEDESC);
	virtual HRESULT __stdcall GetFourCCCodes(LPDWORD, LPDWORD);
	virtual HRESULT __stdcall GetGDISurface(LPDIRECTDRAWSURFACE*);
	virtual HRESULT __stdcall GetMonitorFrequency(LPDWORD);
	virtual HRESULT __stdcall GetScanLine(LPDWORD);
	virtual HRESULT __stdcall GetVerticalBlankStatus(LPBOOL);
	virtual HRESULT __stdcall Initialize(GUID*);
	virtual HRESULT __stdcall RestoreDisplayMode();
	virtual HRESULT __stdcall SetCooperativeLevel(HWND, DWORD);
	virtual HRESULT __stdcall SetDisplayMode(DWORD, DWORD, DWORD);
	virtual HRESULT __stdcall WaitForVerticalBlank(DWORD, HANDLE);

	static BOOL isWindowed();
	static void setWindowed(BOOL);
};

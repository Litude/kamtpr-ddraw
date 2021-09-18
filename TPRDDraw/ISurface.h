#include "stdafx.h"

class ISurface : public IDirectDrawSurface3 {
public:
	/*** IUnknown methods ***/
	virtual HRESULT __stdcall QueryInterface(REFIID, LPVOID*);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	/*** IDirectDrawSurface methods ***/
	virtual HRESULT __stdcall AddAttachedSurface(LPDIRECTDRAWSURFACE3);
	virtual HRESULT __stdcall AddOverlayDirtyRect(LPRECT);
	virtual HRESULT __stdcall Blt(LPRECT, LPDIRECTDRAWSURFACE3, LPRECT, DWORD, LPDDBLTFX);
	virtual HRESULT __stdcall BltBatch(LPDDBLTBATCH, DWORD, DWORD);
	virtual HRESULT __stdcall BltFast(DWORD, DWORD, LPDIRECTDRAWSURFACE3, LPRECT, DWORD);
	virtual HRESULT __stdcall DeleteAttachedSurface(DWORD, LPDIRECTDRAWSURFACE3);
	virtual HRESULT __stdcall EnumAttachedSurfaces(LPVOID, LPDDENUMSURFACESCALLBACK);
	virtual HRESULT __stdcall EnumOverlayZOrders(DWORD, LPVOID, LPDDENUMSURFACESCALLBACK);
	virtual HRESULT __stdcall Flip(LPDIRECTDRAWSURFACE3, DWORD);
	virtual HRESULT __stdcall GetAttachedSurface(LPDDSCAPS, LPDIRECTDRAWSURFACE3*);
	virtual HRESULT __stdcall GetBltStatus(DWORD);
	virtual HRESULT __stdcall GetCaps(LPDDSCAPS);
	virtual HRESULT __stdcall GetClipper(LPDIRECTDRAWCLIPPER*);
	virtual HRESULT __stdcall GetColorKey(DWORD, LPDDCOLORKEY);
	virtual HRESULT __stdcall GetDC(HDC*);
	virtual HRESULT __stdcall GetFlipStatus(DWORD);
	virtual HRESULT __stdcall GetOverlayPosition(LPLONG, LPLONG);
	virtual HRESULT __stdcall GetPalette(LPDIRECTDRAWPALETTE*);
	virtual HRESULT __stdcall GetPixelFormat(LPDDPIXELFORMAT);
	virtual HRESULT __stdcall GetSurfaceDesc(LPDDSURFACEDESC);
	virtual HRESULT __stdcall Initialize(LPDIRECTDRAW, LPDDSURFACEDESC);
	virtual HRESULT __stdcall IsLost();
	virtual HRESULT __stdcall Lock(LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
	virtual HRESULT __stdcall ReleaseDC(HDC);
	virtual HRESULT __stdcall Restore();
	virtual HRESULT __stdcall SetClipper(LPDIRECTDRAWCLIPPER);
	virtual HRESULT __stdcall SetColorKey(DWORD, LPDDCOLORKEY);
	virtual HRESULT __stdcall SetOverlayPosition(LONG, LONG);
	virtual HRESULT __stdcall SetPalette(LPDIRECTDRAWPALETTE);
	virtual HRESULT __stdcall Unlock(LPVOID);
	virtual HRESULT __stdcall UpdateOverlay(LPRECT, LPDIRECTDRAWSURFACE3, LPRECT, DWORD, LPDDOVERLAYFX);
	virtual HRESULT __stdcall UpdateOverlayDisplay(DWORD);
	virtual HRESULT __stdcall UpdateOverlayZOrder(DWORD, LPDIRECTDRAWSURFACE3);
	/*** Added in the v2 interface ***/
	virtual HRESULT __stdcall GetDDInterface(LPVOID*);
	virtual HRESULT __stdcall PageLock(DWORD);
	virtual HRESULT __stdcall PageUnlock(DWORD);
	/*** Added in the V3 interface ***/
	virtual HRESULT __stdcall SetSurfaceDesc(LPDDSURFACEDESC, DWORD);
};

#pragma once
#include "stdafx.h"
#include "ISurface.h"

class WrapperRenderer;
class WrapperPalette;

struct VERTEX {
	float x;
	float y;
	float z;
	float rhw;
	float tu;
	float tv;
};

class WrapperSurface : ISurface {
public:
	/*** IUnknown methods ***/
	virtual HRESULT __stdcall QueryInterface(REFIID riid, LPVOID FAR* ppvObj);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	/*** IDirectDrawSurface methods ***/
	virtual HRESULT __stdcall BltFast(DWORD, DWORD, LPDIRECTDRAWSURFACE3, LPRECT, DWORD);
	virtual HRESULT __stdcall GetAttachedSurface(LPDDSCAPS, LPDIRECTDRAWSURFACE3*);
	virtual HRESULT __stdcall Lock(LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
	virtual HRESULT __stdcall Restore();
	virtual HRESULT __stdcall SetPalette(LPDIRECTDRAWPALETTE);
	virtual HRESULT __stdcall Unlock(LPVOID);

	WrapperSurface(WrapperRenderer* directDraw, LPDDSURFACEDESC lpDDSurfaceDesc);

	DWORD refCount;
	WrapperPalette* palette;
	WrapperRenderer* renderer;
	IDirect3DTexture9* texture;
	BYTE* pixelBuffer;
	DDSURFACEDESC surfaceDescription;
	BOOL deviceLost;
	VERTEX vertices[4];
};

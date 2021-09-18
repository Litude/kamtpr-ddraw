#include "ISurface.h"
class LegacyRenderer;
class WrapperPalette;

class LegacySurface : ISurface {
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

	LegacySurface(LegacyRenderer*, LPDDSURFACEDESC);

	DWORD refCount;
	LegacyRenderer* renderer;
	LegacySurface* backSurface;
	WrapperPalette* palette;
	LPDIRECTDRAWSURFACE3 ddSurface;
	DDSURFACEDESC surfaceDescription;
	LPDIRECTDRAWCLIPPER clipper;
	BYTE* pixelBuffer;
};

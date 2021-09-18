#include "stdafx.h"

class IPalette : public IDirectDrawPalette {
public:
	/*** IUnknown methods ***/
	virtual HRESULT __stdcall QueryInterface(REFIID, LPVOID*);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	/*** IDirectDraw methods ***/
	virtual HRESULT __stdcall GetCaps(LPDWORD);
	virtual HRESULT __stdcall GetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY);
	virtual HRESULT __stdcall Initialize(LPDIRECTDRAW, DWORD, LPPALETTEENTRY);
	virtual HRESULT __stdcall SetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY);
};

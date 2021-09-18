#include "stdafx.h"
#include "IPalette.h"

HRESULT __stdcall IPalette::QueryInterface(REFIID, LPVOID*)
{
	return E_NOTIMPL;
}

ULONG __stdcall IPalette::AddRef()
{
	return E_NOTIMPL;
}

ULONG __stdcall IPalette::Release()
{
	return E_NOTIMPL;
}

HRESULT __stdcall IPalette::GetCaps(LPDWORD)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IPalette::GetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IPalette::Initialize(LPDIRECTDRAW, DWORD, LPPALETTEENTRY)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IPalette::SetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY)
{
	return E_NOTIMPL;
}

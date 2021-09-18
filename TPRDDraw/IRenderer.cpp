#include "stdafx.h"
#include "IRenderer.h"
#include "constants.h"

HRESULT __stdcall IRenderer::QueryInterface(REFIID, LPVOID*) {
	return E_NOTIMPL;
}

ULONG __stdcall IRenderer::AddRef() {
	return E_NOTIMPL;
}

ULONG __stdcall IRenderer::Release() {
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::Compact() {
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::CreateClipper(DWORD, LPDIRECTDRAWCLIPPER*, IUnknown*)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::CreatePalette(DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE*, IUnknown*)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::CreateSurface(LPDDSURFACEDESC, LPDIRECTDRAWSURFACE*, IUnknown*)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::DuplicateSurface(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE*)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::EnumDisplayModes(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::EnumSurfaces(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::FlipToGDISurface()
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::GetCaps(LPDDCAPS, LPDDCAPS)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::GetDisplayMode(LPDDSURFACEDESC lpDDSurfaceDesc)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::GetFourCCCodes(LPDWORD, LPDWORD)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::GetGDISurface(LPDIRECTDRAWSURFACE *lplpGDIDDSSurface)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::GetMonitorFrequency(LPDWORD)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::GetScanLine(LPDWORD)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::GetVerticalBlankStatus(LPBOOL)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::Initialize(GUID*)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::RestoreDisplayMode()
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::SetCooperativeLevel(HWND, DWORD)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::SetDisplayMode(DWORD, DWORD, DWORD)
{
	return E_NOTIMPL;
}

HRESULT __stdcall IRenderer::WaitForVerticalBlank(DWORD, HANDLE)
{
	return E_NOTIMPL;
}

//TODO: These should be moved somewhere else...
BOOL IRenderer::isWindowed() {
#ifdef GENERAL
	return TRUE;
#else
	return (BOOL)*((BYTE*)WINDOWED_ADDRESS);
#endif
}

void IRenderer::setWindowed(BOOL windowed) {
#ifndef GENERAL
	BYTE* windowedValue = (BYTE*)WINDOWED_ADDRESS;
	*windowedValue = (BYTE)windowed;
#endif
}

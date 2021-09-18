#include "stdafx.h"
#include "ISurface.h"

HRESULT __stdcall ISurface::QueryInterface(REFIID riid, LPVOID* ppvObj) {
	return E_NOTIMPL;
}

ULONG __stdcall ISurface::AddRef() {
	return E_NOTIMPL;
}

ULONG __stdcall ISurface::Release() {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::AddAttachedSurface(LPDIRECTDRAWSURFACE3) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::AddOverlayDirtyRect(LPRECT) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::Blt(LPRECT, LPDIRECTDRAWSURFACE3, LPRECT, DWORD, LPDDBLTFX) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::BltBatch(LPDDBLTBATCH, DWORD, DWORD) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::BltFast(DWORD, DWORD, LPDIRECTDRAWSURFACE3, LPRECT, DWORD) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::DeleteAttachedSurface(DWORD, LPDIRECTDRAWSURFACE3) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::EnumAttachedSurfaces(LPVOID, LPDDENUMSURFACESCALLBACK) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::EnumOverlayZOrders(DWORD, LPVOID, LPDDENUMSURFACESCALLBACK) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::Flip(LPDIRECTDRAWSURFACE3, DWORD) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::GetAttachedSurface(LPDDSCAPS, LPDIRECTDRAWSURFACE3*) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::GetBltStatus(DWORD) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::GetCaps(LPDDSCAPS) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::GetClipper(LPDIRECTDRAWCLIPPER*) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::GetColorKey(DWORD, LPDDCOLORKEY) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::GetDC(HDC*) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::GetFlipStatus(DWORD) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::GetOverlayPosition(LPLONG, LPLONG) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::GetPalette(LPDIRECTDRAWPALETTE*) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::GetPixelFormat(LPDDPIXELFORMAT) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::GetSurfaceDesc(LPDDSURFACEDESC) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::Initialize(LPDIRECTDRAW, LPDDSURFACEDESC) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::IsLost() {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::Lock(LPRECT, LPDDSURFACEDESC, DWORD, HANDLE) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::ReleaseDC(HDC) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::Restore() {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::SetClipper(LPDIRECTDRAWCLIPPER) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::SetColorKey(DWORD, LPDDCOLORKEY) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::SetOverlayPosition(LONG, LONG) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::SetPalette(LPDIRECTDRAWPALETTE) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::Unlock(LPVOID) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::UpdateOverlay(LPRECT, LPDIRECTDRAWSURFACE3, LPRECT, DWORD, LPDDOVERLAYFX) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::UpdateOverlayDisplay(DWORD) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::UpdateOverlayZOrder(DWORD, LPDIRECTDRAWSURFACE3) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::GetDDInterface(LPVOID*) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::PageLock(DWORD) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::PageUnlock(DWORD) {
	return E_NOTIMPL;
}

HRESULT __stdcall ISurface::SetSurfaceDesc(LPDDSURFACEDESC, DWORD) {
	return E_NOTIMPL;
}

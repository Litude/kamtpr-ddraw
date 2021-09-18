#include "stdafx.h"
#include "LegacySurface.h"
#include "LegacyRenderer.h"
#include "WrapperPalette.h"
#include "debug.h"

LegacySurface::LegacySurface(LegacyRenderer* renderer, LPDDSURFACEDESC lpDDSurfaceDesc) {
	this->refCount = 1;
	this->ddSurface = NULL;
	this->backSurface = NULL;
	this->palette = NULL;
	this->pixelBuffer = NULL;
	this->clipper = NULL;
	this->renderer = renderer;

	if (this->renderer != NULL) {
		memcpy(&this->surfaceDescription, lpDDSurfaceDesc, sizeof(this->surfaceDescription));
		this->surfaceDescription.dwBackBufferCount = 0;

		if (this->surfaceDescription.dwFlags & DDSD_CAPS && this->surfaceDescription.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE) {
			printDebugString("LegacySurface::LegacySurface Creating primary surface\n");
			this->surfaceDescription.dwFlags = DDSD_CAPS;
			this->surfaceDescription.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			IDirectDrawSurface* surface = NULL;

			if (this->renderer->directDraw->CreateSurface(&this->surfaceDescription, &surface, NULL) != DD_OK) {
				printDebugString("LegacySurface::LegacySurface Creating primary surface through DirectDraw failed\n");
			}

			if (surface->QueryInterface(IID_IDirectDrawSurface3, (LPVOID*)&this->ddSurface) != DD_OK) {
				printDebugString("LegacySurface::LegacySurface Quering for IDirectDrawSurface3 interface failed\n");
			}
			printReleaseCount("IDirectDrawSurface Primary", surface->Release());

			if (this->renderer->directDraw->CreateClipper(0, &clipper, NULL) != DD_OK) {
				printDebugString("LegacySurface::LegacySurface Creating clipper for primary surface through DirectDraw failed\n");
			}

			
			if (clipper != NULL && clipper->SetHWnd(0, this->renderer->windowHandle) != DD_OK) {
				printDebugString("LegacySurface::LegacySurface Setting window handle for clipper failed\n");
			}

			if (this->ddSurface != NULL && this->ddSurface->SetClipper(clipper) != DD_OK) {
				printDebugString("LegacySurface::LegacySurface Setting clipper for primary surface failed\n");
			}

		}
		else {
			printDebugString("LegacySurface::LegacySurface Creating back surface\n");
			this->surfaceDescription.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
			this->surfaceDescription.dwWidth = renderer->width;
			this->surfaceDescription.dwHeight = renderer->height;
			this->surfaceDescription.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
			this->pixelBuffer = new BYTE[this->surfaceDescription.dwWidth * this->surfaceDescription.dwHeight];
			
			IDirectDrawSurface* surface = NULL;

			if (this->renderer->directDraw->CreateSurface(&this->surfaceDescription, &surface, NULL) != DD_OK) {
				printDebugString("LegacySurface::LegacySurface Creating back surface through DirectDraw failed\n");
			}
			if (surface->QueryInterface(IID_IDirectDrawSurface3, (LPVOID*)&this->ddSurface) != DD_OK) {
				printDebugString("LegacySurface::LegacySurface Creating back surface through DirectDraw failed\n");
			}
			printReleaseCount("IDirectDrawSurface Backbuffer", surface->Release());
		}
	}
}

HRESULT __stdcall LegacySurface::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	printDebugString("LegacySurface::QueryInterface called\n");
	*ppvObj = this;
	this->AddRef();
	return DD_OK;
}

ULONG __stdcall LegacySurface::AddRef()
{
	printDebugString("LegacySurface::AddRef called\n");
	printDebugString("LegacySurface::AddRef new count %d\n", this->refCount + 1);
	return ++this->refCount;
}

ULONG __stdcall LegacySurface::Release()
{
	printDebugString("LegacySurface::Release called\n");
	ULONG remainingCount = --this->refCount;
	printDebugString("LegacySurface::Release remaining count %d\n", remainingCount);
	if (!remainingCount) {
		if (this->pixelBuffer) {
			delete this->pixelBuffer;
		}
		if (this->backSurface) {
			this->backSurface->Release();
		}
		if (this->ddSurface) {
			if (this->clipper != NULL) {
				this->ddSurface->SetClipper(NULL);
				printReleaseCount("Clipper", this->clipper->Release());
			}
			printReleaseCount("Surface", this->ddSurface->Release());
		}
		delete(this);
	}
	return remainingCount;
}

HRESULT __stdcall LegacySurface::BltFast(DWORD, DWORD, LPDIRECTDRAWSURFACE3 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans)
{
	if (!this->backSurface) {
		printDebugString("LegacySurface::BltFast called on non-primary surface!\n");
		return DDERR_INVALIDOBJECT;
	}
	POINT point1, point2;
	point1.x = 0;
	point1.y = 0;
	point2.x = this->renderer->width;
	point2.y = this->renderer->height;
	ClientToScreen(this->renderer->windowHandle, &point1);
	ClientToScreen(this->renderer->windowHandle, &point2);
	RECT rect;
	rect.left = point1.x;
    rect.top = point1.y;
    rect.right = point2.x;
    rect.bottom = point2.y;
	return this->ddSurface->Blt(&rect, this->backSurface->ddSurface, lpSrcRect, DDBLT_WAIT, NULL);
}

HRESULT __stdcall LegacySurface::GetAttachedSurface(LPDDSCAPS lpDDSCaps, LPDIRECTDRAWSURFACE3* lplpDDAttachedSurface)
{
	printDebugString("LegacySurface::GetAttachedSurface called\n");
	if (lpDDSCaps->dwCaps & DDSCAPS_BACKBUFFER) {
		if (this->backSurface) {
			*lplpDDAttachedSurface = reinterpret_cast<LPDIRECTDRAWSURFACE3>(this->backSurface);
			return DD_OK;
		} else {
			return DDERR_NOTFOUND;
		}
	} else {
		*lplpDDAttachedSurface = reinterpret_cast<LPDIRECTDRAWSURFACE3>(this);
		return DD_OK;
	}
}

HRESULT __stdcall LegacySurface::Lock(LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent)
{
	if (this->backSurface) {
		printDebugString("LegacySurface::Lock tried locking primary surface!\n");
		return DDERR_INVALIDOBJECT;
	} else {
		memcpy(lpDDSurfaceDesc, &this->surfaceDescription, sizeof(DDSURFACEDESC));
		lpDDSurfaceDesc->dwFlags |= DDSD_PITCH;
		lpDDSurfaceDesc->lPitch = surfaceDescription.dwWidth;
		lpDDSurfaceDesc->lpSurface = pixelBuffer;
	}
	return DD_OK;
}

HRESULT __stdcall LegacySurface::Restore()
{
	printDebugString("LegacySurface::Restore called\n");
	return this->ddSurface->Restore();
}

HRESULT __stdcall LegacySurface::SetPalette(LPDIRECTDRAWPALETTE lpDDPalette)
{
	printDebugString("LegacySurface::SetPalette called\n");
	this->palette = reinterpret_cast<WrapperPalette*>(lpDDPalette);
	return DD_OK;
}

HRESULT __stdcall LegacySurface::Unlock(LPVOID)
{
	if (this->backSurface) {
		printDebugString("LegacySurface::Unlock tried unlocking primary surface!\n");
		return DDERR_INVALIDOBJECT;
	} else {
		DDSURFACEDESC backBufferSurface;
		backBufferSurface.dwSize = sizeof(DDSURFACEDESC);
		HRESULT lockResult = this->ddSurface->Lock(NULL, &backBufferSurface, DDLOCK_WAIT, NULL);
		if (lockResult != DD_OK) {
			printDebugString("LegacySurface::Unlock tried locking back surface but got error code %d\n", lockResult);
			return lockResult;
		}

		if (this->palette->type == PALETTE_15BIT || this->palette->type == PALETTE_16BIT) {
			COL16* surfacePixels = reinterpret_cast<COL16*>(backBufferSurface.lpSurface);

			for (DWORD y = 0; y < backBufferSurface.dwHeight; ++y) {
				for (DWORD x = 0; x < backBufferSurface.dwWidth; ++x) {
					surfacePixels[x + y * (backBufferSurface.lPitch / 2)] = this->palette->palette.entries16[this->pixelBuffer[x + y * surfaceDescription.dwWidth]];
				}
			}

		} else if (this->palette->type == PALETTE_24BIT) {
			COL24* surfacePixels = reinterpret_cast<COL24*>(backBufferSurface.lpSurface);

			for (DWORD y = 0; y < backBufferSurface.dwHeight; ++y) {
				for (DWORD x = 0; x < backBufferSurface.dwWidth; ++x) {
					surfacePixels[x + y * (backBufferSurface.lPitch / 3)] = this->palette->palette.entries24[this->pixelBuffer[x + y * surfaceDescription.dwWidth]];
				}
			}
		
		} else {
			COL32* surfacePixels = reinterpret_cast<COL32*>(backBufferSurface.lpSurface);

			for (DWORD y = 0; y < backBufferSurface.dwHeight; ++y) {
				for (DWORD x = 0; x < backBufferSurface.dwWidth; ++x) {
					surfacePixels[x + y * (backBufferSurface.lPitch / 4)] = this->palette->palette.entries32[this->pixelBuffer[x + y * surfaceDescription.dwWidth]];
				}
			}
		}

		lockResult = this->ddSurface->Unlock(NULL);
		if (lockResult != DD_OK) {
			printDebugString("LegacySurface::Unlock tried unlocking back surface but got error code %d\n", lockResult);
			return lockResult;
		}

		return DD_OK;
	}
}

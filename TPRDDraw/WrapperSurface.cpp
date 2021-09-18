#include "stdafx.h"
#include "WrapperSurface.h"
#include "WrapperPalette.h"
#include "WrapperRenderer.h"
#include "debug.h"

WrapperSurface::WrapperSurface(WrapperRenderer* renderer, LPDDSURFACEDESC lpDDSurfaceDesc) {
	this->refCount = 1;
	this->palette = NULL;
	this->renderer = NULL;
	this->texture = NULL;
	this->pixelBuffer = NULL;
	this->deviceLost = FALSE;

	this->renderer = renderer;
	if (renderer != NULL) {
		memcpy(&this->surfaceDescription, lpDDSurfaceDesc, sizeof(this->surfaceDescription));
		this->surfaceDescription.dwWidth = this->renderer->width;
		this->surfaceDescription.dwHeight = this->renderer->height;
		this->surfaceDescription.lPitch = this->renderer->width;
		this->surfaceDescription.dwFlags |= DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH;
		if (this->renderer->d3ddev->CreateTexture(
			this->surfaceDescription.dwWidth,
			this->surfaceDescription.dwHeight,
			1,
			0,
			D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED,
			&this->texture,
			NULL
		) != D3D_OK) {
			printDebugString("Error creating Direct3D Texture!\n");
		}
		this->pixelBuffer = new BYTE[this->surfaceDescription.dwWidth * this->surfaceDescription.dwHeight];

		// need to slightly offset vertices to account for Direct3D 9 pixel offsetting
		// https://docs.microsoft.com/en-us/windows/win32/direct3d9/directly-mapping-texels-to-pixels
		this->vertices[0].x = -0.5f;
		this->vertices[0].y = -0.5f;
		this->vertices[0].z = 0.5f;
		this->vertices[0].rhw = 1.0f;
		this->vertices[0].tu = 0.0f;
		this->vertices[0].tv = 0.0f;
		
		this->vertices[1].x = static_cast<float>(this->renderer->width) - 0.5f;
		this->vertices[1].y = -0.5f;
		this->vertices[1].z = 0.5f;
		this->vertices[1].rhw = 1.0f;
		this->vertices[1].tu = 1.0f;
		this->vertices[1].tv = 0.0f;
		
		this->vertices[2].x = -0.5f;
		this->vertices[2].y = static_cast<float>(this->renderer->height) - 0.5f;
		this->vertices[2].z = 0.5f;
		this->vertices[2].rhw = 1.0f;
		this->vertices[2].tu = 0.0f;
		this->vertices[2].tv = 1.0f;
		
		this->vertices[3].x = static_cast<float>(this->renderer->width) - 0.5f;
		this->vertices[3].y = static_cast<float>(this->renderer->height) - 0.5f;
		this->vertices[3].z = 0.5f;
		this->vertices[3].rhw = 1.0f;
		this->vertices[3].tu = 1.0f;
		this->vertices[3].tv = 1.0f;
	}
}

HRESULT __stdcall WrapperSurface::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	printDebugString("WrapperSurface::QueryInterface called\n");
	*ppvObj = this;
	this->AddRef();
	return DD_OK;
}

ULONG __stdcall WrapperSurface::AddRef()
{
	return ++this->refCount;
}

ULONG __stdcall WrapperSurface::Release()
{
	printDebugString("WrapperSurface::Release called\n");
	ULONG remainingCount = --this->refCount;
	printDebugString("WrapperSurface::Release remaining count %d\n", remainingCount);
	if (!remainingCount) {
		delete(pixelBuffer);
		pixelBuffer = NULL;
		texture->Release();
		texture = NULL;
		palette->Release();
		palette = NULL;
		renderer = NULL;
		delete(this);
	}
	return remainingCount;
}

HRESULT __stdcall WrapperSurface::BltFast(DWORD, DWORD, LPDIRECTDRAWSURFACE3 lpDDSrcSurface, LPRECT, DWORD)
{
	if (this->deviceLost) {
		return this->Restore();
	}
	if (palette) {
		D3DLOCKED_RECT d3dRect = {};
		HRESULT lockResult = texture->LockRect(
			0,
			&d3dRect,
			NULL,
			0
		);
		if (lockResult != D3D_OK) {
			printDebugString("LockRect Failed, received error code %d!\n", lockResult);
			return lockResult;
		}
		d3dRect.Pitch /= 4; // Fill 4 bytes per iteration
		COL32* buffer = reinterpret_cast<COL32*>(d3dRect.pBits);

		for (DWORD y = 0; y < surfaceDescription.dwHeight; ++y) {
			for (DWORD x = 0; x < surfaceDescription.dwWidth; ++x) {
				buffer[x + y * d3dRect.Pitch] = palette->palette.entries32[pixelBuffer[x + y * surfaceDescription.dwWidth]];
			}
		}

		if (texture->UnlockRect(0) != D3D_OK) {
			printDebugString("UnlockRect Failed!\n");
		}
		if (renderer->d3ddev->BeginScene() != D3D_OK) {
			printDebugString("BeginScene failed!\n");
		}
		if (renderer->d3ddev->SetTexture(0, texture) != D3D_OK) {
			printDebugString("SetTexture failed!\n");
		}
		if (renderer->d3ddev->SetFVF(D3DFVF_TEX1 | D3DFVF_XYZRHW) != D3D_OK) {
			printDebugString("SetFVF failed!\n");
		}
		if (renderer->d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, this->vertices, sizeof(VERTEX)) != D3D_OK) {
			printDebugString("DrawPrimitiveUP failed!\n");
		}
		if (renderer->d3ddev->EndScene() != D3D_OK) {
			printDebugString("EndScene failed!\n");
		}
		HRESULT result = renderer->d3ddev->Present(NULL, NULL, NULL, NULL);
		if (result != D3D_OK) {
			printDebugString("Present failed!\n");
			if (result == D3DERR_DEVICELOST) {
				printDebugString("Detected lost device\n");
				deviceLost = TRUE;
				return DDERR_SURFACELOST;
			}
			return DDERR_GENERIC;
		}

	}
	return DD_OK;
}

HRESULT __stdcall WrapperSurface::GetAttachedSurface(LPDDSCAPS lpDDSCaps, LPDIRECTDRAWSURFACE3* lplpDDAttachedSurface)
{
	printDebugString("WrapperSurface::GetAttachedSurface called\n");
	//The default surface is the back surface in DirectX, so just return the default surface
	*lplpDDAttachedSurface = reinterpret_cast<LPDIRECTDRAWSURFACE3>(this);
	return DD_OK;
}

HRESULT __stdcall WrapperSurface::Lock(LPRECT, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD, HANDLE)
{
	//printDebugString("WrapperSurface::Lock called\n");
	memcpy(lpDDSurfaceDesc, &this->surfaceDescription, sizeof(DDSURFACEDESC));
	lpDDSurfaceDesc->lpSurface = pixelBuffer;
	return DD_OK;
}

HRESULT __stdcall WrapperSurface::Restore()
{
	printDebugString("WrapperSurface::Restore called\n");
	if (deviceLost) {
		HRESULT deviceLostState = this->renderer->d3ddev->TestCooperativeLevel();
		if (this->renderer->d3ddev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
			printDebugString("Resetting lost device\n");
			this->renderer->SetDisplayMode(this->renderer->width, this->renderer->height, 32);
			deviceLost = false;
		} else {
			return DDERR_GENERIC;
		}
	}
	return DD_OK;
}

HRESULT __stdcall WrapperSurface::SetPalette(LPDIRECTDRAWPALETTE lpDDPalette)
{
	printDebugString("WrapperSurface::SetPalette called\n");
	if (this->palette) {
		this->palette->Release();
	}
	WrapperPalette* newPalette = reinterpret_cast<WrapperPalette*>(lpDDPalette);
	this->palette = newPalette;
	if (lpDDPalette) {
		lpDDPalette->AddRef();
	}
	return DD_OK;
}

HRESULT __stdcall WrapperSurface::Unlock(LPVOID)
{
	//printDebugString("WrapperSurface::Unlock called\n");
	return DD_OK;
}

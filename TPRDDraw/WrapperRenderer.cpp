#include "stdafx.h"
#include "WrapperRenderer.h"
#include "WrapperSurface.h"
#include "WrapperPalette.h"
#include "LegacyRenderer.h"
#include "debug.h"
#include "constants.h"

class WrapperSurface;

typedef IDirect3D9*(__stdcall* D3DCPROTO)(UINT);

WrapperRenderer::WrapperRenderer() {
	char dllPath[BUFFER_SIZE];

	this->refCount = 1;
	this->windowHandle = NULL;
	this->d3d = NULL;
	this->d3ddev = NULL;
	this->width = 0;
	this->height = 0;
	this->palette = NULL;
	this->surface = NULL;
	this->previousDisplayMode = UNASSIGNED;
	this->storedWinPos.left = 0;
	this->storedWinPos.top = 0;
	this->storedWinPos.right = 0;
	this->storedWinPos.bottom = 0;
	
	GetSystemDirectoryA(dllPath, BUFFER_SIZE);
	strcat_s(dllPath, "\\d3d9.dll");
	HMODULE direct3D9 = LoadLibraryA(dllPath);
	if (direct3D9 != NULL) {
		D3DCPROTO originalFunction = reinterpret_cast<D3DCPROTO>(GetProcAddress(direct3D9, "Direct3DCreate9"));
		if (originalFunction != NULL) {
			printDebugString("Found Direct3DCreate9\n");
			IDirect3D9* direct3d = originalFunction(D3D_SDK_VERSION);
			if (direct3d != NULL) {
				printDebugString("Called Direct3DCreate9 successfully\n");
				this->d3d = direct3d;
			} else {
				throw std::exception("LegacyRenderer creation failed! Call to Direct3DCreate9 failed");
			}
		} else {
			throw std::exception("WrapperRenderer creation failed! Failed to retrieve address of function Direct3DCreate9");
		}
	} else {
		throw std::exception("WrapperRenderer creation failed! Failed to load library");
	}
}


ULONG __stdcall WrapperRenderer::AddRef() {
	printDebugString("LegacyRenderer::AddRef called\n");
	printDebugString("LegacyRenderer::AddRef new count %d\n", this->refCount + 1);
	return ++this->refCount;
}

ULONG __stdcall WrapperRenderer::Release() {
	printDebugString("WrapperRenderer::Release called\n");
	ULONG remainingCount = --this->refCount;
	printDebugString("WrapperRenderer::Release remaining count %d\n", remainingCount);
	if (!remainingCount) {
		if (this->palette) {
			printReleaseCount("WrapperPalette", this->palette->Release());
		}
		if (this->surface) {
			printReleaseCount("WrapperSurface", this->surface->Release());
			printReleaseCount("WrapperSurface", this->surface->Release()); //Workaround for game not releasing surface on exit
		}
		printReleaseCount("IDirect3DDevice9", this->d3ddev->Release());
		printReleaseCount("IDirect3D9", this->d3d->Release());
		delete(this);
	}
	return remainingCount;
}

HRESULT __stdcall WrapperRenderer::CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE *lplpDDPalette, IUnknown *pUnkOuter)
{
	printDebugString("WrapperRenderer::CreatePalette called\n");
	if (this->palette) {
		this->palette->Release();
		this->palette = NULL;
	}
	this->palette = new WrapperPalette(PALETTE_32BIT);
	this->palette->SetEntries(0, 0, 256, lpColorTable);
	*lplpDDPalette = reinterpret_cast<LPDIRECTDRAWPALETTE>(this->palette);
	return DD_OK;
}

HRESULT __stdcall WrapperRenderer::CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface, IUnknown *pUnkOuter)
{
	printDebugString("WrapperRenderer::CreateSurface called\n");
	if (this->surface) {
		this->surface->Release();
		this->surface = NULL;
	}
	this->surface = new WrapperSurface(this, lpDDSurfaceDesc);
	this->surface->AddRef();
	*lplpDDSurface = reinterpret_cast<LPDIRECTDRAWSURFACE>(this->surface);
	return DD_OK;
}

HRESULT __stdcall WrapperRenderer::EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext, LPDDENUMMODESCALLBACK lpEnumCallback)
{
	printDebugString("WrapperRenderer::EnumDisplayModes called\n");
	UINT modeCount = d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
	printDebugString("GetAdapterModeCount returned %d display modes\n", modeCount);
	D3DDISPLAYMODE displayMode = {};
	DDSURFACEDESC ddrawDisplayMode = {};
	ddrawDisplayMode.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
	ddrawDisplayMode.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8;
	for (UINT i = 0; i < modeCount; ++i) {
		d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &displayMode);
		ddrawDisplayMode.dwWidth = displayMode.Width;
		ddrawDisplayMode.dwHeight = displayMode.Height;
		printDebugString("Enumerating display mode %dx%d\n", ddrawDisplayMode.dwWidth, ddrawDisplayMode.dwHeight);
		lpEnumCallback(&ddrawDisplayMode, lpContext);
	}
	return DD_OK;
}

HRESULT __stdcall WrapperRenderer::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
{
	printDebugString("WrapperRenderer::SetCooperativeLevel called\n");
	this->windowHandle = hWnd;
	return DD_OK;
}

HRESULT __stdcall WrapperRenderer::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBpp)
{
	printDebugString("WrapperRenderer::SetDisplayMode called\n");

	BOOL windowed = IRenderer::isWindowed();
	BOOL changingToWindowed = windowed && this->previousDisplayMode == FULLSCREEN;
	BOOL changingToFullScreen = !windowed && this->previousDisplayMode == WINDOWED;

	if (changingToFullScreen) {
		WINDOWPLACEMENT winPlacement = { sizeof(winPlacement) };
		GetWindowPlacement(windowHandle, &winPlacement);
		storedWinPos = winPlacement.rcNormalPosition;
		printDebugString("Stored window placement %d %d %d %d\n", storedWinPos.left, storedWinPos.top, storedWinPos.right, storedWinPos.bottom);
	}

	this->width = dwWidth;
	this->height = dwHeight;

	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.BackBufferWidth = dwWidth;
	d3dpp.BackBufferHeight = dwHeight;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = windowHandle;
	d3dpp.Windowed = windowed;
	d3dpp.EnableAutoDepthStencil = FALSE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (this->d3ddev) {
		if (this->d3ddev->Reset(&d3dpp) != D3D_OK) {
			printDebugString("Device reset failed\n");
			return DDERR_GENERIC;
		}
	}
	else {
		if (d3d->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			this->windowHandle,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&this->d3ddev
		) != D3D_OK) {
			printDebugString("IDirect3D9 CreateDevice failed, falling back to DirectDraw\n");
			this->d3d->Release();

			//SUPERHACK, overwrite current renderer with LegacyRenderer
			LegacyRenderer* legacyRenderer = new LegacyRenderer();
			legacyRenderer->SetCooperativeLevel(this->windowHandle, 0);

			//compile time assertion to ensure this doesn't break if the classes are changed
			C_ASSERT (sizeof(LegacyRenderer) <= sizeof(WrapperRenderer));
			HRESULT result = legacyRenderer->SetDisplayMode(dwWidth, dwHeight, dwBpp);
			memcpy(this, legacyRenderer, sizeof(*legacyRenderer));
			delete legacyRenderer;
			return result;
		}
	}

	if (windowed) {
		printDebugString("Setting windowed mode\n");

		WINDOWPLACEMENT winPlacement = { sizeof(winPlacement) };
		GetWindowPlacement(windowHandle, &winPlacement);

		LONG windowStyle = WINDOW_STYLE_WINDOWED;
		if (this->previousDisplayMode != WINDOWED) {
			SetWindowLongPtr(windowHandle, GWL_STYLE, windowStyle);
			ShowWindow(windowHandle, SW_SHOWNORMAL);
			winPlacement.rcNormalPosition.left = storedWinPos.left;
			winPlacement.rcNormalPosition.top = storedWinPos.top;
		}
		RECT windowSize;
		windowSize.top = 0;
		windowSize.left = 0;
		windowSize.bottom = dwHeight;
		windowSize.right = dwWidth;
		AdjustWindowRect(&windowSize, windowStyle, FALSE);

		//SetWindowPos doesn't resize if the window is minized; use SetWindowPlacement instead
		winPlacement.rcNormalPosition.right = windowSize.right - windowSize.left + winPlacement.rcNormalPosition.left;
		winPlacement.rcNormalPosition.bottom = windowSize.bottom - windowSize.top + winPlacement.rcNormalPosition.top;

		SetWindowPlacement(windowHandle, &winPlacement);

		//DirectX sets the HWND_TOPMOST flag when entering fullscreen, remove it when changing to windowed
		HWND insertAfter = changingToWindowed ? HWND_NOTOPMOST : HWND_TOP;
		UINT windowFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE;
		if (!changingToWindowed) {
			windowFlags |= SWP_NOZORDER;
		}
		SetWindowPos(windowHandle, insertAfter, 0, 0, 0, 0, windowFlags);

	} else {
		printDebugString("Setting fullscreen mode\n");
		LONG windowStyle = WINDOW_STYLE_FULLSCREEN;
		if (this->previousDisplayMode != FULLSCREEN) {
			SetWindowLongPtr(windowHandle, GWL_STYLE, windowStyle);
		}
	}

	this->previousDisplayMode = windowed ? WINDOWED : FULLSCREEN;

	this->d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	this->d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);

	//show black screen
	this->d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	this->d3ddev->Present(NULL, NULL, NULL, NULL);
	return DD_OK;
}

HRESULT __stdcall WrapperRenderer::WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent)
{
	return DD_OK;
}

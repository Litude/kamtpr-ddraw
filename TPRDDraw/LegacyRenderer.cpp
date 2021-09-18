#include "stdafx.h"
#include "LegacyRenderer.h"
#include "LegacySurface.h"
#include "WrapperPalette.h"
#include "debug.h"
#include "constants.h"
#include "stringLibrary.h"

typedef HRESULT(__stdcall* DDCPROTO)(GUID*, LPDIRECTDRAW*, IUnknown*);

#define MAX_DISPLAY_MODES 100

enum {
	BIT_DEPTH_8,
	BIT_DEPTH_15,
	BIT_DEPTH_16,
	BIT_DEPTH_24,
	BIT_DEPTH_32,
	MAX_BITDEPTHS
} ColorBitDepth;

typedef struct {
	DWORD width;
	DWORD height;
} DisplayMode;

typedef struct {
	DisplayMode displayModes[MAX_BITDEPTHS][MAX_DISPLAY_MODES];
	DWORD numModes[MAX_BITDEPTHS];
} EnumContext;

LegacyRenderer::LegacyRenderer() {
	char dllPath[BUFFER_SIZE];

	this->directDraw = directDraw;
	this->refCount = 1;
	this->windowHandle = NULL;
	this->primarySurface = NULL;
	this->palette = NULL;
	this->width = 0;
	this->height = 0;
	this->cooperativeLevel = 0;
	this->storedWinPos.left = 0;
	this->storedWinPos.top = 0;
	this->storedWinPos.right = 0;
	this->storedWinPos.bottom = 0;
	this->desktopBitCount = 0;
	
	GetSystemDirectoryA(dllPath, BUFFER_SIZE);
	strcat_s(dllPath, "\\ddraw.dll");
	HMODULE directDraw = LoadLibraryA(dllPath);

	if (directDraw != NULL) {
		DDCPROTO originalFunction = reinterpret_cast<DDCPROTO>(GetProcAddress(directDraw, "DirectDrawCreate"));
		if (originalFunction != NULL) {
			printDebugString("Found DirectDrawCreate\n");
			HRESULT result = originalFunction(NULL, &this->directDraw, NULL);
			if (result != DD_OK) {
				throw std::exception("LegacyRenderer creation failed! Call to DirectDrawCreate failed");
			}
		} else {
			throw std::exception("LegacyRenderer creation failed! Failed to retrieve address of function DirectDrawCreate");
		}
	} else {
		throw std::exception("LegacyRenderer creation failed! Failed to load library");
	}
}

ULONG __stdcall LegacyRenderer::AddRef() {
	printDebugString("LegacyRenderer::AddRef called\n");
	printDebugString("LegacyRenderer::AddRef new count %d\n", this->refCount + 1);
	return ++this->refCount;
}

ULONG __stdcall LegacyRenderer::Release() {
	printDebugString("LegacyRenderer::Release called\n");
	ULONG remainingCount = --this->refCount;
	printDebugString("LegacyRenderer::Release remaining count %d\n", remainingCount);
	if (!remainingCount) {
		printDebugString("Destroying LegacyRender\n");
		if (this->primarySurface != NULL) {
			this->primarySurface->Release();
			this->primarySurface->Release(); //Workaround for game bug which doesn't release surfaces on exit
		}
		if (this->palette != NULL) {
			printReleaseCount("Palette", this->palette->Release());
			this->palette = NULL;
		}
		if (this->directDraw) {
			printReleaseCount("DirectDraw", this->directDraw->Release());
		}
		delete(this);
	}
	return remainingCount;
}

HRESULT __stdcall LegacyRenderer::CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE *lplpDDPalette, IUnknown *pUnkOuter)
{
	printDebugString("LegacyRenderer::CreatePalette called\n");
	if (this->isWindowed()) {
		if (this->primarySurface) {
			if (this->palette) {
				this->palette->Release();
				this->palette = NULL;
			}
			DDPIXELFORMAT pixelFormat;
			ZeroMemory(&pixelFormat, sizeof(pixelFormat));
			pixelFormat.dwSize = sizeof(pixelFormat);
			this->primarySurface->ddSurface->GetPixelFormat(&pixelFormat);
			
			PaletteType paletteType = PALETTE_32BIT;
			if (this->desktopBitCount == 8) {
				printDebugString("Detected 8-bit color mode! ABORT ABORT!\n");
			} else if (this->desktopBitCount == 15) {
				printDebugString("Detected 15-bit color mode\n");
				paletteType = PALETTE_15BIT;
			} else if (this->desktopBitCount == 16) {
				printDebugString("Detected 16-bit color mode\n");
				paletteType = PALETTE_16BIT;
			} else if (this->desktopBitCount == 24) {
				printDebugString("Detected 24-bit color mode\n");
				paletteType = PALETTE_24BIT;
			} else {
				printDebugString("Detected %d-bit color mode, using 32-bit palette\n", this->desktopBitCount);
			}

			printDebugString("Bit count: %d\n", pixelFormat.dwRGBBitCount);
			printDebugString("Green bit mask: %x\n", pixelFormat.dwGBitMask);
			
			this->palette = new WrapperPalette(paletteType);
			this->palette->SetEntries(0, 0, 256, lpColorTable);
			*lplpDDPalette = reinterpret_cast<LPDIRECTDRAWPALETTE>(this->palette);
			return DD_OK;
		} else {
			return DDERR_GENERIC;
		}
	} else {
		return this->directDraw->CreatePalette(dwFlags, lpColorTable, lplpDDPalette, pUnkOuter);
	}
}

HRESULT __stdcall LegacyRenderer::CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface, IUnknown *pUnkOuter)
{
	printDebugString("LegacyRenderer::CreateSurface called\n");
	if (this->primarySurface) {
		printDebugString("LegacyRenderer::CreateSurface deleting old surfaces\n");
		if (this->primarySurface->backSurface) {
			this->primarySurface->backSurface->Release();
			this->primarySurface->backSurface = NULL;
		}
		this->primarySurface->Release();
		this->primarySurface = NULL;
	}
	if (this->isWindowed()) {
		printDebugString("LegacyRenderer::CreateSurface detected windowed mode\n");
		LegacySurface* primarySurface = new LegacySurface(this, lpDDSurfaceDesc);
		LPDIRECTDRAWCLIPPER lpDDClipper = NULL;
		if (primarySurface != NULL && primarySurface->ddSurface != NULL && primarySurface->clipper != NULL) {
			this->primarySurface = primarySurface;
			
			DDPIXELFORMAT pixelFormat;
			ZeroMemory(&pixelFormat, sizeof(pixelFormat));
			pixelFormat.dwSize = sizeof(pixelFormat);
			this->primarySurface->ddSurface->GetPixelFormat(&pixelFormat);
			DWORD bitCount = pixelFormat.dwRGBBitCount;
			this->desktopBitCount = bitCount;
			printDebugString("LegacyRenderer::CreateSurface primary surface bit count: %d\n", bitCount);
			if (bitCount == 8) {
				char* errorString = getString(391);
				MessageBoxA(this->windowHandle, errorString == NULL ? "Running in windowed mode requires a desktop color depth of at least 16-bit" : errorString, NULL, MB_OK);
				this->primarySurface->Release();
				this->primarySurface = NULL;
				printDebugString("LegacyRenderer::CreateSurface detected 8-bit color in windowed, falling back to fullscreen\n");
				return this->FallbackToFullscreen(lpDDSurfaceDesc, lplpDDSurface);
			}

			this->primarySurface->AddRef();


			if (lpDDSurfaceDesc->dwBackBufferCount == 1) {
				DDSURFACEDESC surfaceDesc = {};
				memcpy(&surfaceDesc, &primarySurface->surfaceDescription, sizeof(DDSURFACEDESC));
				surfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
				LegacySurface* backSurface = new LegacySurface(this, &surfaceDesc);
				if (backSurface->ddSurface != NULL) {
					this->primarySurface->backSurface = backSurface;
					*lplpDDSurface = reinterpret_cast<LPDIRECTDRAWSURFACE>(primarySurface);
					return DD_OK;
				} else {
					this->primarySurface->Release();
					return DDERR_GENERIC;
				}
			} else {
				printDebugString("LegacySurface::CreateSurface created primary surface ONLY\n");
				*lplpDDSurface = reinterpret_cast<LPDIRECTDRAWSURFACE>(primarySurface);
				return DD_OK;
			}

		} else {
			printDebugString("LegacySurface::CreateSurface failed creating primary surface\n");
			return DDERR_GENERIC;
		}
	} else {
		this->desktopBitCount = 8;
		printDebugString("LegacyRenderer::CreateSurface passing call to DirectDraw\n");
		return this->directDraw->CreateSurface(lpDDSurfaceDesc, lplpDDSurface, pUnkOuter);
	}
}

HRESULT __stdcall LegacyRenderer::EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext, LPDDENUMMODESCALLBACK lpEnumCallback)
{
	printDebugString("LegacyRenderer::EnumDisplayModes called\n");
	EnumContext enumContext = {};
    HRESULT result = this->directDraw->EnumDisplayModes(dwFlags, lpDDSurfaceDesc, &enumContext, &LegacyRenderer::EnumModesCallback);
	if (result == DD_OK) {

		DWORD currentBitDepth = BIT_DEPTH_32;
		switch (this->desktopBitCount) {
			case 8:
				currentBitDepth = BIT_DEPTH_8;
				break;
			case 15:
				currentBitDepth = BIT_DEPTH_15;
				break;
			case 16:
				currentBitDepth = BIT_DEPTH_16;
				break;
			case 24:
				currentBitDepth = BIT_DEPTH_24;
				break;
			case 32:
				currentBitDepth = BIT_DEPTH_32;
				break;
			default:
				printDebugString("LegacyRenderer::EnumDisplayModes unknown desktop bit depth %d, defaulting to 32\n", this->desktopBitCount);
		}
		
		for (DWORD i = 0; i < enumContext.numModes[currentBitDepth]; ++i) {
			printDebugString(
				"LegacySurface::EnumDisplayModes passing %d-bit display mode %dx%d to application\n",
				this->desktopBitCount,
				enumContext.displayModes[currentBitDepth][i].width,
				enumContext.displayModes[currentBitDepth][i].height
			);
			DDSURFACEDESC modeDescription;
			modeDescription.dwSize = sizeof(DDSURFACEDESC);
			modeDescription.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
			modeDescription.dwHeight = enumContext.displayModes[currentBitDepth][i].height;
			modeDescription.dwWidth = enumContext.displayModes[currentBitDepth][i].width;
			modeDescription.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8;
			lpEnumCallback(&modeDescription, lpContext);
		}
	}
	return result;
}

HRESULT __stdcall LegacyRenderer::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
{
	printDebugString("LegacyRenderer::SetCooperativeLevel called\n");
	this->windowHandle = hWnd;
	return DD_OK;
}

HRESULT __stdcall LegacyRenderer::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBpp)
{
	printDebugString("LegacyRenderer::SetDisplayMode called\n");
	this->width = dwWidth;
	this->height = dwHeight;

	BOOL windowed = IRenderer::isWindowed();
	BOOL changingToFullScreen = !windowed && this->cooperativeLevel == COOPERATIVE_LEVEL_WINDOWED;

	if (changingToFullScreen) {
		WINDOWPLACEMENT winPlacement = { sizeof(winPlacement) };
		GetWindowPlacement(windowHandle, &winPlacement);
		storedWinPos = winPlacement.rcNormalPosition;
		printDebugString("Stored window placement %d %d %d %d\n", storedWinPos.left, storedWinPos.top, storedWinPos.right, storedWinPos.bottom);
	}

	if (windowed) {
		printDebugString("Setting windowed mode\n");

		WINDOWPLACEMENT winPlacement = { sizeof(winPlacement) };
		GetWindowPlacement(windowHandle, &winPlacement);

		LONG windowStyle = WINDOW_STYLE_WINDOWED;
		if (this->cooperativeLevel == COOPERATIVE_LEVEL_FULLSCREEN) {
			this->directDraw->RestoreDisplayMode();
			winPlacement.rcNormalPosition.left = storedWinPos.left;
			winPlacement.rcNormalPosition.top = storedWinPos.top;
		}
		if (this->cooperativeLevel != COOPERATIVE_LEVEL_WINDOWED) {
			this->cooperativeLevel = COOPERATIVE_LEVEL_WINDOWED;
			this->directDraw->SetCooperativeLevel(this->windowHandle, this->cooperativeLevel);
			SetWindowLongPtr(windowHandle, GWL_STYLE, windowStyle);
			ShowWindow(windowHandle, SW_SHOWNORMAL);
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
		SetWindowPos(windowHandle, HWND_TOP, 0, 0, 0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		return DD_OK;
	} else {
		printDebugString("Setting fullscreen mode\n");
		if (this->cooperativeLevel != COOPERATIVE_LEVEL_FULLSCREEN) {
			this->cooperativeLevel = COOPERATIVE_LEVEL_FULLSCREEN;
			this->directDraw->SetCooperativeLevel(this->windowHandle, this->cooperativeLevel);
			LONG windowStyle = WINDOW_STYLE_FULLSCREEN;
			SetWindowLongPtr(windowHandle, GWL_STYLE, windowStyle);
		}
		HRESULT result = this->directDraw->SetDisplayMode(dwWidth, dwHeight, dwBpp);
		printDebugString("LegacyRenderer::SetDisplayMode returned %d\n", result);
		return result;
	}
}

HRESULT __stdcall LegacyRenderer::WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent)
{
	return this->directDraw->WaitForVerticalBlank(dwFlags, hEvent);
}

HRESULT LegacyRenderer::FallbackToFullscreen(LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface) {
	IRenderer::setWindowed(FALSE);
	this->SetDisplayMode(this->width, this->height, 8);
	return this->CreateSurface(lpDDSurfaceDesc, lplpDDSurface, NULL);
}

HRESULT __stdcall LegacyRenderer::EnumModesCallback(LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext) {
	EnumContext* context = reinterpret_cast<EnumContext*>(lpContext);
	printDebugString("Enumerating display mode %dx%dx%d\n", lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight, lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount);

	const DWORD bitCount = lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount;
	DWORD colorBitDepth = BIT_DEPTH_32;
	switch (bitCount) {
		case 8:
			colorBitDepth = BIT_DEPTH_8;
			break;
		case 15:
			colorBitDepth = BIT_DEPTH_15;
			break;
		case 16:
			colorBitDepth = BIT_DEPTH_16;
			break;
		case 24:
			colorBitDepth = BIT_DEPTH_24;
			break;
		case 32:
			colorBitDepth = BIT_DEPTH_32;
			break;
		default:
			if (colorBitDepth < 32) {
				printDebugString("Encountered unsupported bit depth %d, skipping\n", colorBitDepth);
				return DDENUMRET_OK;
			}
	}
	if (colorBitDepth == BIT_DEPTH_8 && (!(lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT) || !(lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8))) {
		printDebugString("Skipping 8-bit color depth entry since it is not color indexed\n");
		return DDENUMRET_OK;
	}

	DWORD current = context->numModes[colorBitDepth];
	if (current == MAX_DISPLAY_MODES) {
		printDebugString("Maximum amount of display modes exceeded, skipping\n", colorBitDepth);
		return DDENUMRET_OK;
	}
	context->displayModes[colorBitDepth][current].width = lpDDSurfaceDesc->dwWidth;
	context->displayModes[colorBitDepth][current].height = lpDDSurfaceDesc->dwHeight;
	++context->numModes[colorBitDepth];
	return DDENUMRET_OK;
}

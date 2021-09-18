#include "stdafx.h"
#include "hook.h"
#include "WrapperRenderer.h"
#include "LegacyRenderer.h"
#include "debug.h"
#include "constants.h"
extern "C"
{
	HRESULT __stdcall DirectDrawCreateHook(GUID* lpGUID, LPDIRECTDRAW* lplpDD, IUnknown* pUnkOuter) {
		clearDebugLog();
		printDebugString("DirectDrawCreateHook called\n");

		OSVERSIONINFO osvi;

		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		GetVersionEx(&osvi);

		BOOL isWindowsVistaOrLater = osvi.dwMajorVersion > 5;

		printDebugString("Windows %d.%d detected\n", osvi.dwMajorVersion, osvi.dwMinorVersion);

		if (isWindowsVistaOrLater) {
			try {
				WrapperRenderer* renderer = new WrapperRenderer();
				*lplpDD = reinterpret_cast<LPDIRECTDRAW>(renderer);
#ifdef GENERAL
				return DD_OK;
#else
				return S_FALSE;
#endif
			} catch (std::exception& error) {
				printDebugString("Creating WrapperRenderer failed: %s\n", error.what());
			}
		}

		try {
			LegacyRenderer* renderer = new LegacyRenderer();
			*lplpDD = reinterpret_cast<LPDIRECTDRAW>(renderer);
#ifdef GENERAL
			return DD_OK;
#else
			return S_FALSE;
#endif
		} catch (std::exception& error) {
			printDebugString("Creating LegacyRenderer failed: %s\n", error.what());
		}

		return E_FAIL;
	}
}

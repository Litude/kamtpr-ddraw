#include "IPalette.h"

enum PaletteType { //A-B-G-R
	PALETTE_15BIT, //0-5-5-5 Untested
	PALETTE_16BIT, //0-5-6-5
	PALETTE_24BIT, //0-8-8-8
	PALETTE_32BIT  //8-8-8-8
};

//MSVC 2005 doesn't pad but never versions certainly do
//Don't allow padding since this is copied directly to the buffer
#pragma pack(push)
#pragma pack(1)
typedef struct {
	BYTE b;
	BYTE g;
	BYTE r;
} COL24;
#pragma pack(pop)

typedef DWORD COL32;
typedef WORD COL16;

//These need to hold or we are in trouble
C_ASSERT(sizeof(COL16) == 2);
C_ASSERT(sizeof(COL24) == 3);
C_ASSERT(sizeof(COL32) == 4);

union colorPalette {
	COL32 entries32[256];
	COL24 entries24[256];
	COL16 entries16[256];
};

class WrapperPalette : IPalette {
public:
	/*** IUnknown methods ***/
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	/*** IDirectDraw methods ***/
	virtual HRESULT __stdcall SetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY);

	WrapperPalette(PaletteType type);

	DWORD refCount;
	colorPalette palette;
	PaletteType type;
};

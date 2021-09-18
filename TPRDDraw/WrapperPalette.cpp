#include "stdafx.h"
#include "WrapperPalette.h"
#include "debug.h"

WrapperPalette::WrapperPalette(PaletteType type) {
	this->refCount = 1;
	this->type = type;
}

ULONG __stdcall WrapperPalette::AddRef()
{
	printDebugString("WrapperPalette::AddRef called\n");
	printDebugString("WrapperPalette::AddRef new count %d\n", this->refCount + 1);
	return ++this->refCount;
}

ULONG __stdcall WrapperPalette::Release()
{
	printDebugString("WrapperPalette::Release called\n");
	ULONG remainingCount = --this->refCount;
	printDebugString("WrapperPalette::Release remaining count %d\n", remainingCount);
	if (!remainingCount) {
		delete(this);
	}
	return remainingCount;
}

HRESULT __stdcall WrapperPalette::SetEntries(DWORD, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries)
{
	if (this->type == PALETTE_15BIT) {
		COL16* destination = &this->palette.entries16[dwStartingEntry];
		DWORD remaining = dwCount;
		LPPALETTEENTRY source = lpEntries;
		while (remaining) {
			COL16 value = 0;
			value |= (source->peRed >> 3) << 10;
			value |= (source->peGreen >> 3) << 5;
			value |= (source->peBlue >> 3);
			*destination = value;
			++destination;
			++source;
			--remaining;
		}
		this->palette.entries16[255] = 0xFFFF;
		this->palette.entries16[0] = 0x0000;
	} else if (this->type == PALETTE_16BIT) {
		COL16* destination = &this->palette.entries16[dwStartingEntry];
		DWORD remaining = dwCount;
		LPPALETTEENTRY source = lpEntries;
		while (remaining) {
			COL16 value = 0;
			value |= (source->peRed >> 3) << 11;
			value |= (source->peGreen >> 2) << 5;
			value |= (source->peBlue >> 3);
			*destination = value;
			++destination;
			++source;
			--remaining;
		}
		this->palette.entries16[255] = 0xFFFF;
		this->palette.entries16[0] = 0x0000;
	} else if (this->type == PALETTE_24BIT) {
		COL24* destination = &this->palette.entries24[dwStartingEntry];
		DWORD remaining = dwCount;
		LPPALETTEENTRY source = lpEntries;
		while (remaining) {
			destination->r = source->peRed;
			destination->g = source->peGreen;
			destination->b = source->peBlue;
			++destination;
			++source;
			--remaining;
		}
		this->palette.entries24[255].r = 255;
		this->palette.entries24[255].g = 255;
		this->palette.entries24[255].b = 255;
		this->palette.entries24[0].r = 0;
		this->palette.entries24[0].g = 0;
		this->palette.entries24[0].b = 0;
	} else {
		COL32* destination = &this->palette.entries32[dwStartingEntry];
		DWORD remaining = dwCount;
		LPPALETTEENTRY source = lpEntries;
		while (remaining) {
			COL32 value = 0;
			value |= source->peRed << 16;
			value |= source->peGreen << 8;
			value |= source->peBlue;
			*destination = value;
			++destination;
			++source;
			--remaining;
		}
		this->palette.entries32[255] = 0x00FFFFFF;
		this->palette.entries32[0] = 0x00000000;
	}
	return DD_OK;
}

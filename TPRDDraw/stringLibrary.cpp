#include "stdafx.h"
#include "stringLibrary.h"

char* getString(unsigned short string_id) {
	stringData** dataPtr = (stringData**)((BYTE*)0x566280);
	stringData* data = *dataPtr;
	if (data == NULL || string_id >= data->numStrings || data->header[string_id] == 0xFFFF) {
		return NULL;
	} else {
		return &data->content[data->header[string_id]];
	}
}

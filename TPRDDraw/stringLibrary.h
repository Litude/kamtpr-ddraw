#include "stdafx.h"

//Should never be packed since it needs to match the struct in the original EXE
#pragma pack(push, 1)
typedef struct {
	unsigned short numStrings;
	int contentSize;
	unsigned short* header;
	char* content;
} stringData;
#pragma pack(pop)

C_ASSERT(sizeof(stringData) == 14);

char* getString(unsigned short string_id);

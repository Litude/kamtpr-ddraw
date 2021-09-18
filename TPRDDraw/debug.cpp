#include "stdafx.h"
#include "debug.h"
#if PRINTDEBUG

const char* DEBUG_FILENAME = "ddraw_debug.log";

void clearDebugLog() {
	FILE* stream = NULL;
	fopen_s(&stream, DEBUG_FILENAME, "w");
	if (stream != NULL) {
		fclose(stream);
	}
}

void printDebugString(const char* format, ...) {
	FILE* stream = NULL;
	fopen_s(&stream, DEBUG_FILENAME, "a");
	if (stream != NULL) {
		va_list args;
		va_start(args, format);
		vfprintf(stream, format, args);
		fclose(stream);
        va_end(args);
	}
}

void printReleaseCount(const char* prefix, ULONG count) {
	printDebugString("%s References remaining after release: %u\n", prefix, count);
}

#endif

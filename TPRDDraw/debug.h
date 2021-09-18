#pragma once
#define PRINTDEBUG 1
extern const char* DEBUG_FILENAME;

#if PRINTDEBUG

void clearDebugLog();

//Eliminated in release builds, don't pass any expressions with side effects as parameters
void printDebugString(const char* format, ...);

void printReleaseCount(const char*, ULONG);

#else
inline void clearDebugLog() {};

#define printDebugString(...) {}

inline void printReleaseCount(const char*, ULONG) {};
#endif

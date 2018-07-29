#pragma once
#include "SDK.h"

class Utils {
public:
	void Print(const char* fmt, ...);
	DWORD FindPatternIDA(std::string moduleName, std::string pattern);
	DWORD FindPattern(std::string moduleName, BYTE* Mask, char* szMask);
	void ShowMessagebox(const char* message, const char* title);
}; extern Utils* g_pUtils;
#pragma once
#include "stdafx.h"

DWORD GetProcId(const wchar_t* procName);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

uintptr_t FIndDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

std::string ProcessIdToName(DWORD processId);
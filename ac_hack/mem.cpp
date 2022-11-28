#include "mem.h"

void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess) {
	DWORD oldProtect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldProtect, &oldProtect);
}

void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess) {
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}


/*
//read
DWORD TestDword = ReadMemory<DWORD>(HANDLE, ADDRESS);
int TestInt = ReadMemory<int>(HANDLE, ADDRESS);
float TestFloat = ReadMemory<float>(HANDLE, ADDRESS);
bool TestBool = ReadMemory<bool>(HANDLE, ADDRESS);

//write
float WriteFloatTest = 1.0f;
WriteMemory<float>(HANDLE, ADDRESS, WriteFloatTest);
bool WriteBoolTest = true;
WriteMemory<bool>(HANDLE, ADDRESS, WriteBoolTest);
*/
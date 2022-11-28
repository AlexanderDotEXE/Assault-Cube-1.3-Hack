#include <filesystem>
#include "stdafx.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"
#include "addresses.h"
using namespace std;

template<typename TYPE>
TYPE ReadMemory(HANDLE proc, DWORD address) {
	TYPE buffer;
	ReadProcessMemory(proc, (LPCVOID)address, &buffer, sizeof(buffer), 0);
	return buffer;
}

template<typename TYPE>
void WriteMemory(HANDLE proc, DWORD address, TYPE dataToWrite) {
	TYPE buffer = dataToWrite;
	WriteProcessMemory(proc, (LPVOID)address, &buffer, sizeof(buffer), 0);
}

int main()
{
	//ID von AC bekommen
	DWORD procId = GetProcId(L"ac_client.exe");

	//Base Adress bekommen (warum nícht 4000?)
	uintptr_t moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

	//Handle bekommen
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	//Pointer
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x01A3344;

	//Adresse bekommen
	vector<unsigned int> healthOffsets = { 0x4, 0x64, 0x98, 0x30, 0x164 };
	uintptr_t healthAddress = FIndDMAAddy(hProcess, dynamicPtrBaseAddr, healthOffsets);

	int health = 0;
	ReadProcessMemory(hProcess, (BYTE*)healthAddress, &health, sizeof(health), nullptr);

	int newHealth = 42069;

	bool healthToggle = false;
	while (true) {
		if(GetAsyncKeyState(VK_F1)) healthToggle = !healthToggle;
		if (healthToggle) {
			WriteMemory(hProcess, healthAddress, newHealth);
		}
	}

}
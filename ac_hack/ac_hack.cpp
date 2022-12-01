#include <filesystem>
#include "stdafx.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"
#include "addresses.h"
#include <stdlib.h>
#include "memory.h"
using namespace std;

void clear() {
	// CSI[2J clears screen, CSI[H moves the cursor to top-left corner
	std::cout << "\x1B[2J\x1B[H";
}

void drawInstructions() {
	cout << "F1: GodMode" << endl << "F2: No Recoil" << endl << "F3: Unlimited Ammo" << endl << "F4: SuperJump (Space->Higher/CTRL->Lower)" << endl;
	cout << "F5: Player Pos" << endl << "F6: Rapid Fire" << endl;
}

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

	if (procId == 0 || moduleBase == 0) return 0;

	//Handle bekommen
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	//Pointer 
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x01A3344;
	uintptr_t dynamicPlayerEntityPtr = moduleBase + 0x17E0A8; // x axis
	uintptr_t weaponPtr = moduleBase + 0x17E0A8;
	cout << "Module Base: " << (hex) << moduleBase << endl << "PlayerEntity: " << dynamicPlayerEntityPtr << endl;

	//Adressen bekommen
	vector<unsigned int> healthOffsets = { 0x4, 0x64, 0x98, 0x30, 0x164 };
	vector<unsigned int> entityOffsets = { 0x0 };
	vector<unsigned int> recoil1Offsets = { 0x364, 0xC, 0x5E };
	vector<unsigned int> recoil2Offsets = { 0x364, 0xC, 0x60 };
	vector<unsigned int> currentAmmoOffsets = { 0x368, 0x14, 0x0 };
	vector<unsigned int> fastShootingOffsets = { 0x140 };

	uintptr_t healthAddress = FIndDMAAddy(hProcess, dynamicPtrBaseAddr, healthOffsets);
	uintptr_t entityAddress = FIndDMAAddy(hProcess, dynamicPlayerEntityPtr, entityOffsets);
	uintptr_t recoil1 = FIndDMAAddy(hProcess, weaponPtr, recoil1Offsets);
	uintptr_t recoil2 = FIndDMAAddy(hProcess, weaponPtr, recoil2Offsets);
	uintptr_t currentAmmo = FIndDMAAddy(hProcess, weaponPtr, currentAmmoOffsets);
	uintptr_t rapidFire = currentAmmo + 0x24;
	uintptr_t playerBodyX = entityAddress + 0x4;
	uintptr_t playerBodyY = entityAddress + 0x8;
	uintptr_t playerBodyZ = entityAddress + 0xc;
	uintptr_t playerHeadZ = entityAddress + 0x30;

	//read health
	int health = 0;
	ReadProcessMemory(hProcess, (BYTE*)healthAddress, &health, sizeof(health), nullptr);

	int newHealth = 42069;
	bool healthToggle = false;
	bool recoilToggle = false;
	bool ammoToggle = false;
	bool superJumpToggle = false;
	bool playerPosToggle = false;
	bool rapidFireToggle = false;
	cout << "PlayerBody X (address): " << (hex) << playerBodyX << endl;

	int tick = 50;

	drawInstructions();
	while (true) {
		if (GetAsyncKeyState(VK_F1)) { 
			Sleep(100);
			healthToggle = !healthToggle; 
			if (healthToggle) cout << "GodMode Activated" << endl;
			else { cout << "GodMode Deactivated" << endl; }
		}
		if (healthToggle) {
			WriteMemory(hProcess, healthAddress, 100);
		}
		if (GetAsyncKeyState(VK_F2)) { 
			Sleep(100);
			recoilToggle = !recoilToggle;
			if (recoilToggle) cout << "No Recoil Activated ";
			else cout << "No Recoil Deactivated" << endl;
		}
		if (recoilToggle) {
			WriteMemory(hProcess, recoil1, 0);
			WriteMemory(hProcess, recoil2, 0);
		}
		if (GetAsyncKeyState(VK_F3)) { 
			Sleep(100);
			ammoToggle = !ammoToggle;
			if (ammoToggle) cout << "Unlimited Ammo Activated" << endl;
			else cout << "Unlimited Ammo Deactivated" << endl;
		}
		if (ammoToggle) {
			WriteMemory(hProcess, currentAmmo, 99);
		}
		if (GetAsyncKeyState(VK_F4)) {
			Sleep(100);
			superJumpToggle = !superJumpToggle;
		if (superJumpToggle) cout << "Super Jump Activated" << endl;
		else cout << "Super Jump Deactivated" << endl;
		}
		if (superJumpToggle && GetAsyncKeyState(0x20)) {
			Sleep(100);
			float height = 0;
			ReadProcessMemory(hProcess, (BYTE*)playerHeadZ, &height, sizeof(height), nullptr);
			WriteMemory(hProcess, playerHeadZ, height += 3);
		}
		if (superJumpToggle && GetAsyncKeyState(VK_CONTROL)) {
			Sleep(100);
			float height = 0;
			ReadProcessMemory(hProcess, (BYTE*)playerHeadZ, &height, sizeof(height), nullptr);
			WriteMemory(hProcess, playerHeadZ, height -= 2);
		}
		if (GetAsyncKeyState(VK_F5)) playerPosToggle = !playerPosToggle;
		if (playerPosToggle) {
			drawInstructions();
			float p_x = 0;
			ReadProcessMemory(hProcess, (BYTE*)playerBodyX, &p_x, sizeof(p_x), nullptr);
			cout << "Player X: " << p_x << endl;
			float p_y = 0;
			ReadProcessMemory(hProcess, (BYTE*)playerBodyY, &p_y, sizeof(p_y), nullptr);
			cout << "Player Y: " << p_y << endl;
			float p_z = 0;
			ReadProcessMemory(hProcess, (BYTE*)playerBodyZ, &p_z, sizeof(p_z), nullptr);
			cout << "Player Z: " << p_z << endl;
			clear();
		}
		if (GetAsyncKeyState(VK_F6)) {
			Sleep(100);
			rapidFireToggle = !rapidFireToggle;
			if (rapidFireToggle) cout << "Rapid Fire Activated" << endl;
			else cout << "Rapid Fire Deactivated" << endl;
		}
		if (rapidFireToggle) {
			WriteMemory(hProcess, rapidFire, 0);
		}
		tick < 1 ? tick = 100 : tick--;
	}

} 
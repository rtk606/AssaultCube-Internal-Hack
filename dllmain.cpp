#include <iostream>
#include <vector>
#include <string>

#include <Windows.h>
#include <TlHelp32.h>

#include "process.h"
#include "memory.h"
#include "entity.h"

DWORD WINAPI HackThread(HMODULE hModule) {	
	// Try to allocate a console window for this application
	if (!AllocConsole()) {
		std::cerr << "Error: AllocConsole() failed." << std::endl;
		return 1;
	}

	FILE* file = nullptr;
	// Redirect stdout to the new console
	if (freopen_s(&file, "CONOUT$", "w", stdout) != 0) {
		std::cerr << "Error: freopen_s() failed." << std::endl;
		FreeConsole();
		return 1;
	}
	else {
	// Simple console menu
		std::cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
		std::cout << "*            RTK's Internal Trainer: Assault Cube             *" << std::endl;
		std::cout << "*                    github.com/rtk606                        *" << std::endl;
		std::cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
		std::cout << "*                                                             *" << std::endl;
		std::cout << "*      [NUMPAD1] GOD MODE                                     *" << std::endl;
		std::cout << "*                                                             *" << std::endl;
		std::cout << "*      [NUMPAD2] INF AMMO                                     *" << std::endl;
		std::cout << "*                                                             *" << std::endl;
		std::cout << "*      [NUMPAD3] NO RECOIL                                    *" << std::endl;
		std::cout << "*                                                             *" << std::endl;
		std::cout << "*      [NUMPAD4] SET TELEPORT                                 *" << std::endl;
		std::cout << "*                                                             *" << std::endl;
		std::cout << "*      [NUMPAD5] TELEPORT                                     *" << std::endl;
		std::cout << "*                                                             *" << std::endl;
		std::cout << "*      [NUMPAD6] WALLHACK (TODO)                              *" << std::endl;
		std::cout << "*                                                             *" << std::endl;
		std::cout << "*      [NUMPAD7] AIMBOT (TODO)                                *" << std::endl;
		std::cout << "*                                                             *" << std::endl;
		std::cout << "*                                                             *" << std::endl;
		std::cout << "*      [NUMPAD8] EXIT TRAINER (TODO)                          *" << std::endl;
		std::cout << "*                                                             *" << std::endl;
		std::cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
	}

	// Retrieves the base address of the module 'ac_client.exe' typically loaded at 0x00400000 
	std::uintptr_t moduleBaseAddress = (uintptr_t)GetModuleHandle(L"ac_client.exe");

	// Some variables we'll need
	Player* localPlayer = *(Player**)(moduleBaseAddress + 0x10F4F4);

	bool health   = false;
	bool recoil   = false;
	bool ammo	  = false;
	bool cordsSet = false;

	std::vector<float> coordinates(3);

	// Main hack loop
	while (true) {
		// User numpad key input to enable and disable features
		if (GetAsyncKeyState(VK_END) & 1) {
			break;
		}
		// Set high health
		if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
			health = !health;
		}
		// Set high ammo
		if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
			ammo = !ammo;
		}
		// Set no recoil
		if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
			recoil = !recoil;

			// Replaces the recoil bytes in memory with NOP instructions while enabled, otherwise writes back the original instructions
			if (recoil) {
				Memory::NopBytes(reinterpret_cast<BYTE*>(moduleBaseAddress + 0x63786), 10);
			} else {
				Memory::PatchBytes(reinterpret_cast<BYTE*>(moduleBaseAddress + 0x63786), (BYTE*)("\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2"), 10);
			}
		}
		// Set teleport coordinates
		if (GetAsyncKeyState(VK_NUMPAD4) & 1) {
			if (localPlayer) {
				coordinates[0] = localPlayer->posCoord.x;
				coordinates[1] = localPlayer->posCoord.y;
				coordinates[2] = localPlayer->posCoord.z;

				cordsSet = true;

				std::cout << "X: " << localPlayer->posCoord.x << "| Y: " << localPlayer->posCoord.y << "| Z: " << localPlayer->posCoord.z << std::endl;
			}
		}

		if (GetAsyncKeyState(VK_NUMPAD5) & 1) {
			if (cordsSet) {
				localPlayer->posCoord.x = coordinates[0];
				localPlayer->posCoord.y = coordinates[1];
				localPlayer->posCoord.z = coordinates[2];

				std::cout << "Teleport activated" << std::endl;
			}
		}

		// This if check ensures we only execute when we're in-game and not in the main menu
		if (localPlayer) {
			// Sets player health to 9999
			if (health) {
				localPlayer->health = 9999;
			}
			// Sets player ammo to 9999
			if (ammo) {
				*(localPlayer->currentWeaponPtr->ammo) = 9999;
			}
		}
		Sleep(30);
	}

	// Clean up resources and exit
	if (file) {
		fclose(file);
	}
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	
	return 0;
}

// Once the DLL is loaded into the process (attached) we create a new thread that runs the HackThread function
BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
		case DLL_THREAD_ATTACH:  break;
		case DLL_THREAD_DETACH:  break;
		case DLL_PROCESS_ATTACH: CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)(HackThread), hModule, 0, nullptr); break;
		case DLL_PROCESS_DETACH: break;
	}

	return TRUE;
}
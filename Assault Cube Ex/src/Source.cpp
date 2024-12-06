#include "stdafx.h"
#include "process_manip.h"
#include "memory_manip.h"
#include "game_offsets.h"
#include "game_values.h"
#include "console.h"
#include <string>

DWORD WaitForProcess(const wchar_t* processName)
{
	DWORD processID{ 0 };
	do
	{
		Notice noticeWaitingForGame("Waiting for process", TextColors::BRIGHT_RED);
		processID = process_manip::GetProcessID(processName);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		system("cls");
	} while (processID == 0);

	return processID;
}

uintptr_t WaitForModule(DWORD processID, const wchar_t* moduleName)
{
	uintptr_t moduleBaseAddress{ 0 };
	do
	{
		Notice noticeWaitingForModule("Waiting for the module to finish loading", TextColors::GREEN);
		moduleBaseAddress = process_manip::GetProcessModuleBaseAddress(processID, moduleName);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		system("cls");
	} while (moduleBaseAddress == 0);

	return moduleBaseAddress;
}

HANDLE OpenTargetProcess(DWORD processID) {
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, NULL, processID);
	if (!process) {
		throw std::runtime_error("Failed to open process. Error code: " + std::to_string(GetLastError()));
	}
	return process;
}

void ToggleCheat(HANDLE process, uintptr_t addr, int& newValue, int& oldValue, bool& toggleState) {
	toggleState = !toggleState;
	memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(addr),
		reinterpret_cast<BYTE*>(toggleState ? &newValue : &oldValue), sizeof(int));
}


int main()
{
	// Wait for the process and module

	DWORD processID = WaitForProcess(L"ac_client.exe");
	uintptr_t moduleBaseAddress = WaitForModule(processID, L"ac_client.exe");

	std::cout << "Process ID: " << std::hex << processID << '\n';
	std::cout << "Module base address: " << moduleBaseAddress << std::dec << '\n';


	// Open handle to the process

	HANDLE process = OpenTargetProcess(processID);
	if (process != NULL)
	{
		// Get the addresses for each cheat

		uintptr_t localPlayerAddr		= moduleBaseAddress + game_offsets::relative;
		uintptr_t healthAddr			= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::health);
		uintptr_t armorAddr				= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::armor);
		uintptr_t primaryMagAddr		= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryMag);
		uintptr_t primaryAmmoAddr		= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryAmmo);
		uintptr_t primaryStateAddr		= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryState);
		uintptr_t grenadesAddr			= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::grenades);
		uintptr_t recoilNopAddr			= moduleBaseAddress + 0x63786;


		// Declare and initalize variables for things such as health

		int currentHealth{ 0 };
		int currentArmor{ 0 };
		int currentPrimaryMag{ 0 };
		int currentPrimaryAmmo{ 0 };
		int currentPrimaryState{ 0 };
		int currentGrenades{ 0 };


		// Booleans control whether the cheat is toggled or not

		bool health = false;
		bool armor = false;
		bool primaryMag = false;
		bool primaryAmmo = false;
		bool grenades = false;
		bool recoil = false;

		DWORD exitcode = 0;
		while (GetExitCodeProcess(process, &exitcode) && exitcode == STILL_ACTIVE && !GetAsyncKeyState(VK_END))
		{
			Notice noticeQuitKey("Game running, press the ", TextColors::BRIGHT_WHITE, "End", TextColors::BRIGHT_RED, " key to exit the console");
			Notice noticeNumLockKey("Ensure your ", TextColors::BRIGHT_RED, "Num Lock", TextColors::BRIGHT_GREEN, " key is toggled");
			std::cout << "\n";
			Notice noticeHealthKey("Use the ", TextColors::GRAY, "Numpad 9", TextColors::BRIGHT_GREEN, " key to set Health");
			Notice noticeArmorKey("Use the ", TextColors::GRAY, "Numpad 8", TextColors::BRIGHT_GREEN, " key to set Armor");
			Notice noticePrimaryMagKey("Use the ", TextColors::GRAY, "Numpad 7", TextColors::BRIGHT_GREEN, " key to set Primary magazine");
			Notice noticePrimaryAmmoKey("Use the ", TextColors::GRAY, "Numpad 6", TextColors::BRIGHT_GREEN, " key to set Primary ammunition");
			Notice noticeGrenadesKey("Use the ", TextColors::GRAY, "Numpad 5", TextColors::BRIGHT_GREEN, " key to set Grenades");
			Notice noticeRecoilKey{"Use the ", TextColors::GRAY, "Numpad 4", TextColors::BRIGHT_GREEN, " key to set Recoil" };
			std::cout << "\n";

			if (GetAsyncKeyState(VK_NUMPAD9) & 0x0001) ToggleCheat(process, healthAddr, game_values::newHealth, game_values::oldHealth, health);
			else if (GetAsyncKeyState(VK_NUMPAD8) & 0x0001) ToggleCheat(process, armorAddr, game_values::newArmor, game_values::oldArmor, armor);
			else if (GetAsyncKeyState(VK_NUMPAD7) & 0x0001) ToggleCheat(process, primaryMag, game_values::newPrimaryMag, game_values::oldPrimaryMag, primaryMag);
			else if (GetAsyncKeyState(VK_NUMPAD6) & 0x0001) ToggleCheat(process, primaryAmmoAddr, game_values::newPrimaryAmmo, game_values::oldPrimaryAmmo, primaryAmmo);
			else if (GetAsyncKeyState(VK_NUMPAD5) & 0x0001) ToggleCheat(process, grenadesAddr, game_values::newGrenades, game_values::oldGrenades, grenades);
			else if (GetAsyncKeyState(VK_NUMPAD4) & 0x0001)
			{
				recoil = !recoil;
				if (recoil)
				{
					memory_manip::NopEx(process, reinterpret_cast<BYTE*>(recoilNopAddr), 10);
				}
				else
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(recoilNopAddr), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10);
				}
			}


			// Update the variables for health and other

			ReadProcessMemory(process, reinterpret_cast<BYTE*>(healthAddr), &currentHealth, sizeof(int), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(armorAddr), &currentArmor, sizeof(int), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(primaryMagAddr), &currentPrimaryMag, sizeof(int), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(primaryAmmoAddr), &currentPrimaryAmmo, sizeof(int), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(primaryStateAddr), &currentPrimaryState, sizeof(int), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(grenadesAddr), &currentGrenades, sizeof(int), nullptr);


			// Update the console with new variable values

			ValueData noticeHealth("Health", healthAddr, &currentHealth, TextColors::BRIGHT_GREEN);
			ValueData noticeArmor("Armor", armorAddr, &currentArmor, TextColors::GREEN);
			ValueData noticePrimaryMag("Primary Mag", primaryMagAddr, &currentPrimaryMag, TextColors::BRIGHT_RED);
			ValueData noticePrimaryAmmo("Primary Ammo", primaryAmmoAddr, &currentPrimaryAmmo, TextColors::BRIGHT_RED);
			ValueData noticeprimaryStateAddr("Primary State", primaryStateAddr, &currentPrimaryState, TextColors::GRAY);
			ValueData noticeGrenades("Grenades", grenadesAddr, &currentGrenades, TextColors::BRIGHT_RED);

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			system("cls");
		}
	}
	else
	{
		std::cerr << "Error, invalid handle value when attempting to OpenProcess\n";
		static_cast<void>(std::getchar());
		return EXIT_FAILURE;
	}

	CloseHandle(process);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	return EXIT_SUCCESS;
}
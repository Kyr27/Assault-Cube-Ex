#include "stdafx.h"
#include "process_manip.h"
#include "memory_manip.h"
#include "game_offsets.h"
#include "game_values.h"
#include "console.h"
#include <iomanip>


int main()
{

	std::chrono::time_point<std::chrono::system_clock>start, end;
	DWORD processID{ 0 };
	do
	{
		Notice noticeWaitingForGame("Waiting for ac_client.exe", TextColors::BRIGHT_RED);
		processID = process_manip::GetProcessID(L"ac_client.exe");
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		system("cls");
	} while (processID == 0);

	uintptr_t moduleBaseAddress{ 0 };
	do
	{
		Notice noticeWaitingForModule("Waiting for the module to finish loading", TextColors::GREEN);
		moduleBaseAddress = process_manip::GetProcessModuleBaseAddress(processID, L"ac_client.exe");
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		system("cls");
	} while (moduleBaseAddress == 0);

	std::cout << "Process ID: " << std::hex << processID << '\n';
	std::cout << "Module base address: " << moduleBaseAddress << std::dec << '\n';

	uintptr_t localPlayerAddr = moduleBaseAddress + game_offsets::relative;

	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, NULL, processID);
	if (process != NULL)
	{
		uintptr_t healthAddr			= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::health);
		uintptr_t armorAddr				= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::armor);
		uintptr_t primaryMagAddr		= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryMag);
		uintptr_t primaryAmmoAddr		= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryAmmo);
		uintptr_t primaryStateAddr		= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryState);
		uintptr_t grenadesAddr			= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::grenades);
		uintptr_t recoilNopAddr			= moduleBaseAddress + 0x63786;

		bool health = false;
		bool armor = false;
		bool primaryMag = false;
		bool primaryAmmo = false;
		bool grenades = false;
		bool recoil = false;

		DWORD exitcode = 0;
		start = std::chrono::system_clock::now();
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

			if (GetAsyncKeyState(VK_NUMPAD9) & 0x0001)
			{
				health = !health;
				if (health)
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(healthAddr), reinterpret_cast<BYTE*>(&game_values::newHealth), sizeof(game_values::newHealth));
				}
				else
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(healthAddr), reinterpret_cast<BYTE*>(&game_values::oldHealth), sizeof(game_values::oldHealth));
				}
			}
			else if (GetAsyncKeyState(VK_NUMPAD8) & 0x0001)
			{
				armor = !armor;
				if (armor)
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(armorAddr), reinterpret_cast<BYTE*>(&game_values::newArmor), sizeof(game_values::newArmor));
				}
				else
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(armorAddr), reinterpret_cast<BYTE*>(&game_values::oldArmor), sizeof(game_values::oldArmor));
				}
			}
			else if (GetAsyncKeyState(VK_NUMPAD7) & 0x0001)
			{
				primaryMag = !primaryMag;
				if (primaryMag)
				{
					//memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(primaryMagAddr), reinterpret_cast<BYTE*>(&game_values::newPrimaryMag), sizeof(game_values::newPrimaryMag));
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(moduleBaseAddress + 0x637e9), (BYTE*)"\xFF\x06", 2);
				}
				else
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(moduleBaseAddress + 0x637e9), (BYTE*)"\xFF\x0E", 2);
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(primaryMagAddr), reinterpret_cast<BYTE*>(&game_values::oldPrimaryMag), sizeof(game_values::oldPrimaryMag));
				}
			}
			else if (GetAsyncKeyState(VK_NUMPAD6) & 0x0001)
			{
				primaryAmmo = !primaryAmmo;
				if (primaryAmmo)
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(primaryAmmoAddr), reinterpret_cast<BYTE*>(&game_values::newPrimaryAmmo), sizeof(game_values::newPrimaryAmmo));
				}
				else
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(primaryAmmoAddr), reinterpret_cast<BYTE*>(&game_values::oldPrimaryAmmo), sizeof(game_values::oldPrimaryAmmo));
				}
			}
			else if (GetAsyncKeyState(VK_NUMPAD5) & 0x0001)
			{
				grenades = !grenades;
				if (grenades)
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(grenadesAddr), reinterpret_cast<BYTE*>(&game_values::newGrenades), sizeof(game_values::newGrenades));
				}
				else
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(grenadesAddr), reinterpret_cast<BYTE*>(&game_values::oldGrenades), sizeof(game_values::oldGrenades));
				}
			}
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
			
			int health			{ 0 };
			int armor			{ 0 };
			int primaryMag		{ 0 };
			int primaryAmmo		{ 0 };
			int primaryState	{ 0 };
			int grenades		{ 0 };

			ReadProcessMemory(process, reinterpret_cast<BYTE*>(healthAddr), &health, sizeof(health), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(armorAddr), &armor, sizeof(armor), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(primaryMagAddr), &primaryMag, sizeof(primaryMag), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(primaryAmmoAddr), &primaryAmmo, sizeof(primaryAmmo), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(primaryStateAddr), &primaryState, sizeof(primaryState), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(grenadesAddr), &grenades, sizeof(grenades), nullptr);

			ValueData noticeHealth("Health", healthAddr, &health, TextColors::BRIGHT_GREEN);
			ValueData noticeArmor("Armor", armorAddr, &armor, TextColors::GREEN);
			ValueData noticePrimaryMag("Primary Mag", primaryMagAddr, &primaryMag, TextColors::BRIGHT_RED);
			ValueData noticePrimaryAmmo("Primary Ammo", primaryAmmoAddr, &primaryAmmo, TextColors::BRIGHT_RED);
			ValueData noticeprimaryStateAddr("Primary State", primaryStateAddr, &primaryState, TextColors::GRAY);
			ValueData noticeGrenades("Grenades", grenadesAddr, &grenades, TextColors::BRIGHT_RED);

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			system("cls");
		}
	}
	else
	{
		std::cerr << "Error, invalid handle value when attempting to OpenProcess\n";
		std::getchar();
		return EXIT_FAILURE;
	}

	CloseHandle(process);
	end = std::chrono::system_clock::now();
	std::chrono::duration<double>elapsedMinutes = (end - start)/60;
	std::cout << "Runtime duration: " << elapsedMinutes.count() << " min" << '\n';
	std::this_thread::sleep_for(std::chrono::seconds(2));
	return EXIT_SUCCESS;
}
#include "stdafx.h"
#include "processassist.h"
#include "memoryassist.h"
#include "game_offsets.h"
#include "game_values.h"
#include "console.h"
#include "cheatassist.h"

int main()
{
	try {
		// Wait for process and module to load

		DWORD processID = ProcessAssist::WaitForProcess(L"ac_client.exe");
		uintptr_t moduleBase = ProcessAssist::WaitForModule(processID, L"ac_client.exe");

		std::cout << "Process ID: " << std::hex << processID << '\n';
		std::cout << "Module base address: " << moduleBase << '\n';


		// Open a handle to the game so we can modify it

		HANDLE process = ProcessAssist::OpenTargetProcess(processID);


		// Get the addresses for each cheat

		uintptr_t localPlayerAddr = moduleBase + game_offsets::relative;
		uintptr_t healthAddr = ProcessAssist::GetStaticPointer(process, localPlayerAddr, game_offsets::health);
		uintptr_t armorAddr = ProcessAssist::GetStaticPointer(process, localPlayerAddr, game_offsets::armor);
		uintptr_t primaryMagAddr = ProcessAssist::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryMag);
		uintptr_t primaryAmmoAddr = ProcessAssist::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryAmmo);
		uintptr_t primaryStateAddr = ProcessAssist::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryState);
		uintptr_t grenadesAddr = ProcessAssist::GetStaticPointer(process, localPlayerAddr, game_offsets::grenades);
		uintptr_t recoilNopAddr = moduleBase + 0x63786;


		// Declare and initialize the cheat variables

		std::vector<FeatureToggle> cheats = {
			{"Health", ProcessAssist::GetStaticPointer(process, localPlayerAddr, game_offsets::health), game_values::newHealth, game_values::oldHealth, false},
			{"Armor", ProcessAssist::GetStaticPointer(process, localPlayerAddr, game_offsets::armor), game_values::newArmor, game_values::oldArmor, false},
			{"Primary Mag", ProcessAssist::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryMag), game_values::newPrimaryMag, game_values::oldPrimaryMag, false},
			{"Primary Ammo", ProcessAssist::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryAmmo), game_values::newPrimaryAmmo, game_values::oldPrimaryAmmo, false},
			{"Grenades", ProcessAssist::GetStaticPointer(process, localPlayerAddr, game_offsets::grenades), game_values::newGrenades, game_values::oldGrenades, false},
		};

		CheatAssist cheatHelper(process);


		// Listen for keystrokes, and then activate and display the cheat that corresponds to the pressed key

		DWORD exitCode = 0;
		while (GetExitCodeProcess(process, &exitCode) && exitCode == STILL_ACTIVE && !GetAsyncKeyState(VK_END)) // Run the loop only while the game is running by checking the exitCode
		{
			// Check if the key matches any cheat toggle key, if it does then toggle it
			HandleInput(process, cheatHelper, cheats);

			// Display and update the cheat values
			for (const auto& feature : cheats) {
				int value = cheatHelper.ReadFeatureValue(feature);
				DisplayValue(feature.name, feature.address, value, TextColors::BRIGHT_GREEN);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}

		// Close the handle to prevent a memory leak
		CloseHandle(process);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
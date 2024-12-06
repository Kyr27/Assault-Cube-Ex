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

template <typename T>
T ReadValue(HANDLE process, uintptr_t addr) {
	T value;
	ReadProcessMemory(process, reinterpret_cast<BYTE*>(addr), &value, sizeof(T), nullptr);
	return value;
}

void DisplayValue(const std::string& name, uintptr_t addr, int value, TextColors color) {
	ValueData notice(name, addr, &value, color);
}

struct FeatureToggle {
	std::string name;
	uintptr_t address;
	int newValue;
	int oldValue;
	bool isEnabled;
};

class CheatAssist {
public:
	CheatAssist(HANDLE proc) : process(proc) {}

	void ToggleFeature(FeatureToggle& feature) {
		feature.isEnabled = !feature.isEnabled;
		memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(feature.address),
			reinterpret_cast<BYTE*>(feature.isEnabled ? &feature.newValue : &feature.oldValue),
			sizeof(int));
	}

	int ReadFeatureValue(const FeatureToggle& feature) {
		return ReadValue<int>(process, feature.address);
	}

private:
	HANDLE process;
};

void HandleInput(HANDLE process, CheatAssist& cheatHelper, std::vector<FeatureToggle>& features) {
	for (size_t i = 0; i < features.size(); ++i) {
		if (GetAsyncKeyState(VK_NUMPAD9 - i) & 0x0001) {
			cheatHelper.ToggleFeature(features[i]);
		}
	}
}


int main()
{
	try {
		// Wait for process and module to load

		DWORD processID = WaitForProcess(L"ac_client.exe");
		uintptr_t moduleBase = WaitForModule(processID, L"ac_client.exe");

		std::cout << "Process ID: " << std::hex << processID << '\n';
		std::cout << "Module base address: " << moduleBase << '\n';


		// Open a handle to the game so we can modify it

		HANDLE process = OpenTargetProcess(processID);


		// Get the addresses for each cheat

		uintptr_t localPlayerAddr = moduleBase + game_offsets::relative;
		uintptr_t healthAddr = process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::health);
		uintptr_t armorAddr = process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::armor);
		uintptr_t primaryMagAddr = process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryMag);
		uintptr_t primaryAmmoAddr = process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryAmmo);
		uintptr_t primaryStateAddr = process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryState);
		uintptr_t grenadesAddr = process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::grenades);
		uintptr_t recoilNopAddr = moduleBase + 0x63786;


		// Declare and initialize the cheat variables

		std::vector<FeatureToggle> cheats = {
			{"Health", process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::health), game_values::newHealth, game_values::oldHealth, false},
			{"Armor", process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::armor), game_values::newArmor, game_values::oldArmor, false},
			{"Primary Mag", process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryMag), game_values::newPrimaryMag, game_values::oldPrimaryMag, false},
			{"Primary Ammo", process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryAmmo), game_values::newPrimaryAmmo, game_values::oldPrimaryAmmo, false},
			{"Grenades", process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::grenades), game_values::newGrenades, game_values::oldGrenades, false},
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
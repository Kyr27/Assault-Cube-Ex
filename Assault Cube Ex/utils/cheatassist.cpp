#include "cheatassist.h"

void ToggleCheat(HANDLE process, uintptr_t addr, int& newValue, int& oldValue, bool& toggleState) {
	toggleState = !toggleState;
	MemoryAssist::PatchEx(process, reinterpret_cast<BYTE*>(addr),
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

CheatAssist::CheatAssist(HANDLE proc) : process(proc) {}

void CheatAssist::ToggleFeature(FeatureToggle& feature) {
	feature.isEnabled = !feature.isEnabled;
	MemoryAssist::PatchEx(process, reinterpret_cast<BYTE*>(feature.address),
		reinterpret_cast<BYTE*>(feature.isEnabled ? &feature.newValue : &feature.oldValue),
		sizeof(int));
}

int CheatAssist::ReadFeatureValue(const FeatureToggle& feature) {
	return ReadValue<int>(process, feature.address);
}

void HandleInput(HANDLE process, CheatAssist& cheatHelper, std::vector<FeatureToggle>& features) {
	for (size_t i = 0; i < features.size(); ++i) {
		if (GetAsyncKeyState(VK_NUMPAD9 - i) & 0x0001) {
			cheatHelper.ToggleFeature(features[i]);
		}
	}
}
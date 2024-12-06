#pragma once
#include "stdafx.h"
#include "console.h"
#include "memoryassist.h"

void ToggleCheat(HANDLE process, uintptr_t addr, int& newValue, int& oldValue, bool& toggleState) {
	toggleState = !toggleState;
	MemoryAssist::PatchEx(process, reinterpret_cast<BYTE*>(addr),
		reinterpret_cast<BYTE*>(toggleState ? &newValue : &oldValue), sizeof(int));
}

template <typename T>
T ReadValue(HANDLE process, uintptr_t addr);

void DisplayValue(const std::string& name, uintptr_t addr, int value, TextColors color);

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
	void ToggleFeature(FeatureToggle& feature);
	int ReadFeatureValue(const FeatureToggle& feature);

private:
	HANDLE process;
};

void HandleInput(HANDLE process, CheatAssist& cheatHelper, std::vector<FeatureToggle>& features);
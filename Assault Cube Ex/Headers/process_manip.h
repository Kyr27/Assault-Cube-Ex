#pragma once
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

namespace process_manip
{
	// Traverses a list of processes and compares their executable file name against given name(case insensitive)
	// Returns the process ID of that process if successful
	// If the function fails the return value is zero
	DWORD GetProcessID(const wchar_t* processName);

	// Traverses a list of modules within a given process and compares their module name against given module name(case insensitive)
	// If the function succeeds it returns the module base address of that process
	// If the function fails the return value is zero
	uintptr_t GetProcessModuleBaseAddress(DWORD processID, const wchar_t* moduleName);

	// Adds offsets together for a pointer within targeted process to obtain a static pointer
	uintptr_t GetStaticPointer(HANDLE process, uintptr_t pointer, std::vector<unsigned int>offsets);
}
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

namespace ProcessAssist
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

	// Wait for the process and return its ID once it has been opened
	DWORD WaitForProcess(const wchar_t* processName);

	// Wait for the module to load, and return its start address
	uintptr_t WaitForModule(DWORD processID, const wchar_t* moduleName);

	// Get a handle to the given process, throws if it fails
	HANDLE OpenTargetProcess(DWORD processID);
}
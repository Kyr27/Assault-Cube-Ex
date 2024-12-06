#define WIN32_LEAN_AND_MEAN
#include "process_manip.h"
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

namespace process_manip
{
	DWORD GetProcessID(const wchar_t* processName)
	{
		DWORD processID{ 0 };

		HANDLE processSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (processSnap != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 pe32;
			pe32.dwSize = sizeof(PROCESSENTRY32);

			if (Process32First(processSnap, &pe32))
			{
				do
				{
					if (!_wcsicmp(pe32.szExeFile, processName))
					{
						processID = pe32.th32ProcessID;
						break;
					}

				} while (Process32Next(processSnap, &pe32));
			}
		}
		CloseHandle(processSnap);
		return processID;
	}
	uintptr_t GetProcessModuleBaseAddress(DWORD processID, const wchar_t* moduleName)
	{
		uintptr_t moduleBaseAddress{ 0 };

		HANDLE processSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, processID);
		if (processSnap != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 me32;
			me32.dwSize = sizeof(MODULEENTRY32);

			if (Module32First(processSnap, &me32))
			{
				do
				{
					if (!_wcsicmp(me32.szModule, moduleName))
					{
						moduleBaseAddress = reinterpret_cast<uintptr_t>(me32.modBaseAddr);
						break;
					}
				} while (Module32Next(processSnap, &me32));
			}
		}
		CloseHandle(processSnap);
		return moduleBaseAddress;
	}
	uintptr_t GetStaticPointer(HANDLE process, uintptr_t pointer, std::vector<unsigned int>offsets)
	{
		for (auto offset : offsets)
		{
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(pointer), &pointer, sizeof(pointer), nullptr);
			pointer += offset;
		}
		return pointer;
	}
}
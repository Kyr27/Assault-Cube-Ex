#include "stdafx.h"
#include "memory_manip.h"

namespace memory_manip
{
	void PatchEx(HANDLE process, BYTE* destination, BYTE* source, size_t size)
	{
		DWORD oldprotect;
		VirtualProtectEx(process, destination, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		WriteProcessMemory(process, destination, source, size, nullptr);
		VirtualProtectEx(process, destination, size, oldprotect, &oldprotect);
	}
	void NopEx(HANDLE process, BYTE* destination, size_t size)
	{
		BYTE* buffer = new BYTE[size];
		memset(buffer, 0x90, size);

		PatchEx(process, destination, buffer, size);
		delete[] buffer;
	}
}
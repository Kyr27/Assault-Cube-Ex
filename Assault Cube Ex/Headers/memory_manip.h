#pragma once
#include <iostream>
#include <Windows.h>

namespace memory_manip
{
	void PatchEx(HANDLE process, BYTE* destination, BYTE* source, size_t size);
	void NopEx(HANDLE process, BYTE* destination, size_t size);
}
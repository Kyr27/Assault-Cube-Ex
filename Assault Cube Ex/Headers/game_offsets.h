#include <vector>

namespace game_offsets
{
	unsigned int relative{ 0x10f4f4 };

	std::vector<unsigned int>health				{ 0xF8 };
	std::vector<unsigned int>armor				{ 0xFC };

	std::vector<unsigned int>grenades			{ 0x158 };
	std::vector<unsigned int>primaryMag			{ 0x150 };
	std::vector<unsigned int>primaryAmmo		{ 0x128 };
	std::vector<unsigned int>primaryState		{ 0x178 };
	std::vector<unsigned int>secondaryMag		{ 0x13C };
	std::vector<unsigned int>secondaryAmmo		{ 0x114 };
	std::vector<unsigned int>secondaryCooldown	{ 0x164 };

	//const char* originalRecoilArray = "\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2";
}
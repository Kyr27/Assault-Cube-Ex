#include "stdafx.h"
#include "process_manip.h"
#include "memory_manip.h"
#include "game_offsets.h"
#include "game_values.h"
#include <iomanip>

enum class TextColors
{
	BRIGHT_RED,
	BRIGHT_WHITE,
	BRIGHT_GREEN,
	GRAY,
	BLACK,
	GREEN
};

enum class ButtonPosition
{
	LEFTMOST,
	RIGHTMOST
};

//Forward declare this in a header, and make another .cpp file with this in it
//Make ValueData aswell as Notice, inherit the button from Console
namespace console_manip
{
	class Console
	{
	protected:
		TextColors m_messageColor{};
		std::string m_message{};
		bool m_button{};
		bool m_buttonEndLine{};
		std::string m_buttonName{};
		TextColors m_buttonColor{};
		std::string m_messageAfterButton{};
		void SetButtonName(std::string buttonName)
		{
			m_buttonName = buttonName;
			if (buttonName.size() > 0)
			{
				m_button = true;
			}
		}
		void SetButtonColor(TextColors buttonColor)
		{
			m_buttonColor = buttonColor;
		}
		void DisplayButton()
		{
			SetTextColor(m_buttonColor);
			if (m_buttonEndLine)
			{
				std::cout << "[" << m_buttonName << "]" << '\n';
			}
			else
			{
				std::cout << "[" << m_buttonName << "]";
			}
		}
	public:
		void SetTextColor(TextColors color)
		{
			switch (color)
			{
			case TextColors::BRIGHT_RED:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			case TextColors::BRIGHT_WHITE:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			case TextColors::BRIGHT_GREEN:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;
			case TextColors::GRAY:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				break;
			case TextColors::BLACK:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), NULL);
				break;
			case TextColors::GREEN:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
				break;
			default:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				break;
			}
		}

		Console()
		{
			m_messageColor = TextColors::BRIGHT_WHITE;
			m_message = "";
			m_button = false;
			m_buttonEndLine = true;
			m_buttonName = "";
			m_buttonColor = TextColors::BRIGHT_GREEN;
			m_messageAfterButton = "";
		}
	};
	class ValueData : public Console
	{
	private:
		uintptr_t m_valueAddress{};
		ButtonPosition m_buttonPosition{};
		int m_value{};
		using Console::SetTextColor;
	public:
		using Console::SetButtonName;
		using Console::SetButtonColor;

		void SetColor(TextColors color)
		{
			m_messageColor = color;
		}
		void SetValueName(std::string valueName)
		{
			m_message = valueName;
		}
		void SetValueAddress(uintptr_t valueAddress)
		{
			m_valueAddress = valueAddress;
		}
		void SetValue(int& value)
		{
			m_value = value;
		}
		void SetButtonPosition(ButtonPosition buttonPosition)
		{
			m_buttonPosition = buttonPosition;
			if (buttonPosition == ButtonPosition::RIGHTMOST)
			{
				m_buttonEndLine = true;
			}
		}
		void DisplayValueData()
		{
			SetTextColor(m_messageColor);

			if (m_message.size() > 0)
			{
				if (m_button && m_buttonPosition == ButtonPosition::LEFTMOST)
				{
					std::cout << m_message << " ";
					DisplayButton();
				}
				else
				{
					std::cout << m_message << ": ";
				}
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			if (m_button && m_buttonPosition == ButtonPosition::RIGHTMOST)
			{
				if (m_value && m_valueAddress)
				{
					std::cout << std::dec << std::setw(10) << m_value << " Address: " << std::hex << m_valueAddress << " ";
					DisplayButton();
				}
				else if (!m_value && m_valueAddress)
				{
					std::cout << " Address: " << std::hex << m_valueAddress << " ";
					DisplayButton();
				}
				else if (m_value && !m_valueAddress)
				{
					std::cout << std::dec << std::setw(10) << m_value << " ";
					DisplayButton();
				}
				else
				{
					std::cerr << "No value/address given\n";
				}
			}
			else
			{
				if (m_value && m_valueAddress)
				{
					std::cout << std::dec << std::setw(10) << m_value << " Address: " << std::hex << m_valueAddress << '\n';
				}
				else if (!m_value && m_valueAddress)
				{
					std::cout << " Address: " << std::hex << m_valueAddress << '\n';
				}
				else if (m_value && !m_valueAddress)
				{
					std::cout << std::dec << std::setw(10) << m_value << '\n';
				}
				else
				{
					std::cerr << "No value/address given\n";
				}
			}
		}

		ValueData(std::string valueName = "", uintptr_t valueAddress = NULL, int* value = nullptr, TextColors color = TextColors::BRIGHT_GREEN,
			std::string buttonName = "", TextColors buttonColor = TextColors::BRIGHT_GREEN,
			ButtonPosition buttonPosition = ButtonPosition::LEFTMOST)
		{
			m_message = valueName;
			m_valueAddress = valueAddress;
			m_messageColor = color;
			if (value)
			{
				m_value = *value;
			}
			else
			{
				m_value = 0;
			}
			if (buttonName.size() > 0)
			{
				m_button = true;
				m_buttonName = buttonName;
			}
			m_buttonPosition = buttonPosition;
			if (m_buttonPosition == ButtonPosition::LEFTMOST)
			{
				m_buttonEndLine = false;
			}
			m_buttonColor = buttonColor;
			if (m_message.size() > 0)
			{
				DisplayValueData();
			}
		}
	};
	class Notice : public Console
	{
	private:
		using Console::SetTextColor;
	public:
		using Console::SetButtonName;
		using Console::SetButtonColor;

		void SetMessage(std::string message)
		{
			m_message = message;
		}
		void SetMessageColor(TextColors messageColor)
		{
			m_messageColor = messageColor;
		}
		void SetMessageAfterButton(std::string afterButtonMessage)
		{
			m_messageAfterButton = afterButtonMessage;
			m_buttonEndLine = false;
		}
		void DisplayNotice()
		{
			SetTextColor(TextColors::BRIGHT_GREEN);
			if (m_button && m_buttonEndLine)
			{
				std::cout << "Notice: ";
				SetTextColor(m_messageColor);
				std::cout << m_message;
				DisplayButton();
			}
			else if (m_button && !m_buttonEndLine)
			{
				std::cout << "Notice: ";
				SetTextColor(m_messageColor);
				std::cout << m_message;
				DisplayButton();
				SetTextColor(m_messageColor);
				std::cout << m_messageAfterButton << '\n';
			}
			else
			{
				std::cout << "Notice: ";
				SetTextColor(m_messageColor);
				std::cout << m_message << '\n';
			}
			SetTextColor(TextColors::GRAY);
		}
		Notice(std::string message = "", TextColors messageColor = TextColors::BRIGHT_WHITE, std::string buttonName = "", TextColors buttonColor = TextColors::BRIGHT_GREEN,
			std::string messageAfterButton = "")
		{
			m_message = message;
			m_messageColor = messageColor;
			if (buttonName.size() > 0)
			{
				m_button = true;
			}
			if (messageAfterButton.size() > 0)
			{
				m_buttonEndLine = false;
			}
			m_buttonName = buttonName;
			m_buttonColor = buttonColor;
			m_messageAfterButton = messageAfterButton;
			if (m_message.size() > 0)
			{
				DisplayNotice();
			}
		}
	};
}

int main()
{

	std::chrono::time_point<std::chrono::system_clock>start, end;
	DWORD processID{ 0 };
	do
	{
		console_manip::Notice noticeWaitingForGame("Waiting for ac_client.exe", TextColors::BRIGHT_RED);
		processID = process_manip::GetProcessID(L"ac_client.exe");
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		system("cls");
	} while (processID == 0);

	uintptr_t moduleBaseAddress{ 0 };
	do
	{
		console_manip::Notice noticeWaitingForModule("Waiting for the module to finish loading", TextColors::GREEN);
		moduleBaseAddress = process_manip::GetProcessModuleBaseAddress(processID, L"ac_client.exe");
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		system("cls");
	} while (moduleBaseAddress == 0);

	std::cout << "Process ID: " << std::hex << processID << '\n';
	std::cout << "Module base address: " << moduleBaseAddress << std::dec << '\n';

	uintptr_t localPlayerAddr = moduleBaseAddress + game_offsets::relative;

	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, NULL, processID);
	if (process != NULL)
	{
		uintptr_t healthAddr			= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::health);
		uintptr_t armorAddr				= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::armor);
		uintptr_t primaryMagAddr		= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryMag);
		uintptr_t primaryAmmoAddr		= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryAmmo);
		uintptr_t primaryStateAddr		= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::primaryState);
		uintptr_t grenadesAddr			= process_manip::GetStaticPointer(process, localPlayerAddr, game_offsets::grenades);
		uintptr_t recoilNopAddr			= moduleBaseAddress + 0x63786;

		bool health = false;
		bool armor = false;
		bool primaryMag = false;
		bool primaryAmmo = false;
		bool grenades = false;
		bool recoil = false;

		DWORD exitcode = 0;
		start = std::chrono::system_clock::now();
		while (GetExitCodeProcess(process, &exitcode) && exitcode == STILL_ACTIVE && !GetAsyncKeyState(VK_END))
		{
			console_manip::Notice noticeQuitKey("Game running, press the ", TextColors::BRIGHT_WHITE, "End", TextColors::BRIGHT_RED, " key to exit the console");
			console_manip::Notice noticeNumLockKey("Ensure your ", TextColors::BRIGHT_RED, "Num Lock", TextColors::BRIGHT_GREEN, " key is toggled");
			std::cout << "\n";
			console_manip::Notice noticeHealthKey("Use the ", TextColors::GRAY, "Numpad 9", TextColors::BRIGHT_GREEN, " key to set Health");
			console_manip::Notice noticeArmorKey("Use the ", TextColors::GRAY, "Numpad 8", TextColors::BRIGHT_GREEN, " key to set Armor");
			console_manip::Notice noticePrimaryMagKey("Use the ", TextColors::GRAY, "Numpad 7", TextColors::BRIGHT_GREEN, " key to set Primary magazine");
			console_manip::Notice noticePrimaryAmmoKey("Use the ", TextColors::GRAY, "Numpad 6", TextColors::BRIGHT_GREEN, " key to set Primary ammunition");
			console_manip::Notice noticeGrenadesKey("Use the ", TextColors::GRAY, "Numpad 5", TextColors::BRIGHT_GREEN, " key to set Grenades");
			console_manip::Notice noticeRecoilKey{"Use the ", TextColors::GRAY, "Numpad 4", TextColors::BRIGHT_GREEN, " key to set Recoil" };
			std::cout << "\n";

			if (GetAsyncKeyState(VK_NUMPAD9) & 0x0001)
			{
				health = !health;
				if (health)
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(healthAddr), reinterpret_cast<BYTE*>(&game_values::newHealth), sizeof(game_values::newHealth));
				}
				else
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(healthAddr), reinterpret_cast<BYTE*>(&game_values::oldHealth), sizeof(game_values::oldHealth));
				}
			}
			else if (GetAsyncKeyState(VK_NUMPAD8) & 0x0001)
			{
				armor = !armor;
				if (armor)
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(armorAddr), reinterpret_cast<BYTE*>(&game_values::newArmor), sizeof(game_values::newArmor));
				}
				else
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(armorAddr), reinterpret_cast<BYTE*>(&game_values::oldArmor), sizeof(game_values::oldArmor));
				}
			}
			else if (GetAsyncKeyState(VK_NUMPAD7) & 0x0001)
			{
				primaryMag = !primaryMag;
				if (primaryMag)
				{
					//memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(primaryMagAddr), reinterpret_cast<BYTE*>(&game_values::newPrimaryMag), sizeof(game_values::newPrimaryMag));
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(moduleBaseAddress + 0x637e9), (BYTE*)"\xFF\x06", 2);
				}
				else
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(moduleBaseAddress + 0x637e9), (BYTE*)"\xFF\x0E", 2);
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(primaryMagAddr), reinterpret_cast<BYTE*>(&game_values::oldPrimaryMag), sizeof(game_values::oldPrimaryMag));
				}
			}
			else if (GetAsyncKeyState(VK_NUMPAD6) & 0x0001)
			{
				primaryAmmo = !primaryAmmo;
				if (primaryAmmo)
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(primaryAmmoAddr), reinterpret_cast<BYTE*>(&game_values::newPrimaryAmmo), sizeof(game_values::newPrimaryAmmo));
				}
				else
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(primaryAmmoAddr), reinterpret_cast<BYTE*>(&game_values::oldPrimaryAmmo), sizeof(game_values::oldPrimaryAmmo));
				}
			}
			else if (GetAsyncKeyState(VK_NUMPAD5) & 0x0001)
			{
				grenades = !grenades;
				if (grenades)
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(grenadesAddr), reinterpret_cast<BYTE*>(&game_values::newGrenades), sizeof(game_values::newGrenades));
				}
				else
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(grenadesAddr), reinterpret_cast<BYTE*>(&game_values::oldGrenades), sizeof(game_values::oldGrenades));
				}
			}
			else if (GetAsyncKeyState(VK_NUMPAD4) & 0x0001)
			{
				recoil = !recoil;
				if (recoil)
				{
					memory_manip::NopEx(process, reinterpret_cast<BYTE*>(recoilNopAddr), 10);
				}
				else
				{
					memory_manip::PatchEx(process, reinterpret_cast<BYTE*>(recoilNopAddr), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10);
				}
			}
			
			int health			{ 0 };
			int armor			{ 0 };
			int primaryMag		{ 0 };
			int primaryAmmo		{ 0 };
			int primaryState	{ 0 };
			int grenades		{ 0 };

			ReadProcessMemory(process, reinterpret_cast<BYTE*>(healthAddr), &health, sizeof(health), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(armorAddr), &armor, sizeof(armor), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(primaryMagAddr), &primaryMag, sizeof(primaryMag), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(primaryAmmoAddr), &primaryAmmo, sizeof(primaryAmmo), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(primaryStateAddr), &primaryState, sizeof(primaryState), nullptr);
			ReadProcessMemory(process, reinterpret_cast<BYTE*>(grenadesAddr), &grenades, sizeof(grenades), nullptr);

			console_manip::ValueData noticeHealth("Health", healthAddr, &health, TextColors::BRIGHT_GREEN);
			console_manip::ValueData noticeArmor("Armor", armorAddr, &armor, TextColors::GREEN);
			console_manip::ValueData noticePrimaryMag("Primary Mag", primaryMagAddr, &primaryMag, TextColors::BRIGHT_RED);
			console_manip::ValueData noticePrimaryAmmo("Primary Ammo", primaryAmmoAddr, &primaryAmmo, TextColors::BRIGHT_RED);
			console_manip::ValueData noticeprimaryStateAddr("Primary State", primaryStateAddr, &primaryState, TextColors::GRAY);
			console_manip::ValueData noticeGrenades("Grenades", grenadesAddr, &grenades, TextColors::BRIGHT_RED);

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			system("cls");
		}
	}
	else
	{
		std::cerr << "Error, invalid handle value when attempting to OpenProcess\n";
		std::getchar();
		return EXIT_FAILURE;
	}
	CloseHandle(process);
	end = std::chrono::system_clock::now();
	std::chrono::duration<double>elapsedMinutes = (end - start)/60;
	std::cout << "Runtime duration: " << elapsedMinutes.count() << " min" << '\n';
	std::this_thread::sleep_for(std::chrono::seconds(2));
	return EXIT_SUCCESS;
}
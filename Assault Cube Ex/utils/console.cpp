#include "console.h"


Console::Console()
	: m_messageColor(TextColors::BRIGHT_WHITE),
	m_message(""),
	m_button(false),
	m_buttonEndLine(true),
	m_buttonName(""),
	m_buttonColor(TextColors::BRIGHT_GREEN),
	m_messageAfterButton("")
{}

void Console::SetTextColor(TextColors color)
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

void Console::SetButtonName(std::string buttonName)
{
	m_buttonName = buttonName;
	if (!buttonName.empty())
	{
		m_button = true;
	}
}

void Console::SetButtonColor(TextColors buttonColor)
{
	m_buttonColor = buttonColor;
}

void Console::DisplayButton()
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

void ValueData::SetColor(TextColors color)
{
	m_messageColor = color;
}

void ValueData::SetValueName(std::string valueName)
{
	m_message = valueName;
}

void ValueData::SetValueAddress(uintptr_t valueAddress)
{
	m_valueAddress = valueAddress;
}

void ValueData::SetValue(int& value)
{
	m_value = value;
}

void ValueData::SetButtonPosition(ButtonPosition buttonPosition)
{
	m_buttonPosition = buttonPosition;
	m_buttonEndLine = (buttonPosition == ButtonPosition::RIGHTMOST);
}

void ValueData::DisplayValueData()
{
	SetTextColor(m_messageColor);

	if (!m_message.empty())
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

ValueData::ValueData(std::string valueName, uintptr_t valueAddress, int* value, TextColors color,
	std::string buttonName, TextColors buttonColor, ButtonPosition buttonPosition)
	: Console()
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
	if (!buttonName.empty())
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
	if (!m_message.empty())
	{
		DisplayValueData();
	}
}

void Notice::SetMessage(std::string message)
{
	m_message = message;
}

void Notice::SetMessageColor(TextColors messageColor)
{
	m_messageColor = messageColor;
}

void Notice::SetMessageAfterButton(std::string afterButtonMessage)
{
	m_messageAfterButton = afterButtonMessage;
	m_buttonEndLine = false;
}

void Notice::DisplayNotice()
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

Notice::Notice(std::string message, TextColors messageColor, std::string buttonName,
	TextColors buttonColor, std::string messageAfterButton)
	: Console()
{
	SetMessage(message);
	SetMessageColor(messageColor);
	if (!buttonName.empty())
	{
		m_button = true;
	}
	if (!messageAfterButton.empty())
	{
		m_buttonEndLine = false;
	}
	m_buttonName = buttonName;
	m_buttonColor = buttonColor;
	m_messageAfterButton = messageAfterButton;

	if (!m_message.empty())
	{
		DisplayNotice();
	}
}

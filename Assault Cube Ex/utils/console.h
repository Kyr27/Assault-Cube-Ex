#pragma once
#include "stdafx.h"

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

	void SetButtonName(std::string buttonName);
	void SetButtonColor(TextColors buttonColor);
	void DisplayButton();
public:
	Console();
	void SetTextColor(TextColors color);
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

	void SetColor(TextColors color);
	void SetValueName(std::string valueName);
	void SetValueAddress(uintptr_t valueAddress);
	void SetValue(int& value);
	void SetButtonPosition(ButtonPosition buttonPosition);
	void DisplayValueData();

	ValueData(std::string valueName = "", uintptr_t valueAddress = NULL, int* value = nullptr, TextColors color = TextColors::BRIGHT_GREEN,
		std::string buttonName = "", TextColors buttonColor = TextColors::BRIGHT_GREEN,
		ButtonPosition buttonPosition = ButtonPosition::LEFTMOST);
};

class Notice : public Console
{
private:
	using Console::SetTextColor;

public:
	using Console::SetButtonName;
	using Console::SetButtonColor;

	void SetMessage(std::string message);
	void SetMessageColor(TextColors messageColor);
	void SetMessageAfterButton(std::string afterButtonMessage);
	void DisplayNotice();

	Notice(std::string message = "", TextColors messageColor = TextColors::BRIGHT_WHITE,
		std::string buttonName = "", TextColors buttonColor = TextColors::BRIGHT_GREEN,
		std::string messageAfterButton = "");
};

#pragma once
#include <cstdint>
#include <sstream>	
#include <iostream>
#include <cstring>
#include <windows.h>

namespace Log
{
	enum class LogType : uint8_t
	{
		L_INFO = 0,
		L_WARNING,
		L_ERROR,
	};

	extern int LogLineNumber;

	void Init();

	template<LogType logType>
	void Print(const std::string& msg)
	{
		SYSTEMTIME localTime;
		GetLocalTime(&localTime);

		HANDLE hConsole = GetStdHandle(-11);

		switch (logType)
		{
		case LogType::L_INFO:
			SetConsoleTextAttribute(hConsole, 10);
			break;
		case LogType::L_WARNING:
			SetConsoleTextAttribute(hConsole, 14);
			break;
		case LogType::L_ERROR:
			SetConsoleTextAttribute(hConsole, 12);
			break;
		};

		std::cout << LogLineNumber++ << ": " << localTime.wDay << "/" << localTime.wMonth << "/" << localTime.wYear << " " << localTime.wHour << ":" << localTime.wMinute << ":" << localTime.wSecond << " => " << msg << std::endl;
	}

	template<LogType logType>
	void Print(std::ostringstream& stream)
	{
		std::string s = stream.str();
		Print<logType>(s);
	}


	//////////////////////////////////////////
	// Shorthands for the template function //
	//////////////////////////////////////////

	// Print Info
	void Print(const std::ostringstream& stream);
	void Print(const std::string& msg);
	void Print(const char* const* msg);

	// Print Warning
	void PrintWarning(const std::ostringstream& stream);
	void PrintWarning(const std::string& msg);
	void PrintWarning(const char* const* msg);

	// Print Error
	void PrintError(const std::ostringstream& stream);
	void PrintError(const std::string& msg);
	void PrintError(const char* const* msg);
}

#include "Log.h"

int Log::LogLineNumber = 0;

void Log::Init()
{
}

void Log::Print(const std::ostringstream& stream)
{
	const std::string s = stream.str();
	Print<Log::LogType::L_INFO>(s);
}

void Log::Print(const std::string& msg)
{
	Print<Log::LogType::L_INFO>(msg);
}

void Log::Print(const char* const* const msg)
{
	const std::string s = *msg;
	Print<Log::LogType::L_INFO>(s);
}


void Log::PrintWarning(const std::ostringstream& stream)
{
	const std::string s = stream.str();
	Print<Log::LogType::L_WARNING>(s);
}

void Log::PrintWarning(const std::string& msg)
{
	Print<Log::LogType::L_WARNING>(msg);
}

void Log::PrintWarning(const char* const* const msg)
{
	const std::string s = *msg;
	Print<Log::LogType::L_WARNING>(s);
}


void Log::PrintError(const std::ostringstream& stream)
{
	const std::string s = stream.str();
	Print<Log::LogType::L_ERROR>(s);
}

void Log::PrintError(const std::string& msg)
{
	Print<Log::LogType::L_ERROR>(msg);
}

void Log::PrintError(const char* const* const msg)
{
	const std::string s = *msg;
	Print<Log::LogType::L_ERROR>(s);
}

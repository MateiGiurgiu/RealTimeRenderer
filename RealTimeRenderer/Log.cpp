#include "Log.h"

int Log::LogLineNumber = 0;

void Log::Init()
{
}

void Log::Print(const std::ostringstream& stream)
{
	const std::string s = stream.str();
	Print<Log::L_INFO>(s);
}

void Log::Print(const std::string& msg)
{
	Print<Log::L_INFO>(msg);
}

void Log::Print(const char** msg)
{
	const std::string s = *msg;
	Print<Log::L_INFO>(s);
}


void Log::PrintWarning(const std::ostringstream& stream)
{
	const std::string s = stream.str();
	Print<Log::L_WARNING>(s);
}

void Log::PrintWarning(const std::string& msg)
{
	Print<Log::L_WARNING>(msg);
}

void Log::PrintWarning(const char** msg)
{
	const std::string s = *msg;
	Print<Log::L_WARNING>(s);
}


void Log::PrintError(const std::ostringstream& stream)
{
	std::string s = stream.str();
	Print<Log::L_ERROR>(s);
}

void Log::PrintError(const std::string& msg)
{
	Print<Log::L_ERROR>(msg);
}

void Log::PrintError(const char** msg)
{
	std::string s = *msg;
	Print<Log::L_ERROR>(s);
}

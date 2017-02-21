#include "HoloLensCppModulesPch.h"
#include "Logger.h"

namespace HoloLensCppModules
{
	void Logger::Log(const char* message)
	{
		std::string str = std::string(message);
		std::wstring wstr = std::wstring(str.begin(), str.end());
		OutputDebugString(wstr.c_str());
	}

	void Logger::Log(std::string message)
	{
		std::wstring wstr = std::wstring(message.begin(), message.end());
		OutputDebugString(wstr.c_str());
	}

	void Logger::Log(std::wstring message)
	{
		OutputDebugString(message.c_str());
	}

	void Logger::Log(Platform::String^ message)
	{
		auto wstr = std::wstring(message->Data()).append(L"\r\n");
		OutputDebugString(wstr.c_str());
	}

	void Logger::Log(int message)
	{
		auto wstr = std::wstring(message.ToString()->Data()).append(L"\r\n");
		OutputDebugString(wstr.c_str());
	}
}
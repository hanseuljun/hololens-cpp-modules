#pragma once

#include <string>

namespace HoloLensCppModules
{
	class Logger
	{
	private:
		Logger()
		{
		}

	public:
		static void Log(const char* message);
		static void Log(std::string message);
		static void Log(std::wstring message);
		static void Log(Platform::String^ message);
		static void Log(int message);
	};
}
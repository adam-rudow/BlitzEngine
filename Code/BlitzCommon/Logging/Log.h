#pragma once

#include <memory.h>
#include <string.h>

#include "spdlog/spdlog.h"

class Log
{
public:
	Log();
	~Log();

	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return sCoreLogger; }
	inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return sClientLogger; }

private:
	static std::shared_ptr<spdlog::logger> sCoreLogger; 
	static std::shared_ptr<spdlog::logger> sClientLogger;
};

#define __FILENAME__ (std::strrchr("\\" __FILE__, '\\') + 1)

#define BZ_CORE_TRACE(...)   ::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define BZ_CORE_INFO(...)    ::Log::GetCoreLogger()->info(__VA_ARGS__)
#define BZ_CORE_WARN(...)    ::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define BZ_CORE_ERROR(...)   ::Log::GetCoreLogger()->error(__VA_ARGS__)
#define BZ_CORE_FATAL(...)   ::Log::GetCoreLogger()->critical(__VA_ARGS__); __debugbreak();
#define BZ_ASSERT(expr, ...) if( !(expr) ) { BZ_CORE_FATAL(__VA_ARGS__); }

#define BZ_TRACE(...)        ::Log::GetClientLogger()->trace(__VA_ARGS__)
#define BZ_INFO(...)         ::Log::GetClientLogger()->info(__VA_ARGS__)
#define BZ_WARN(...)         ::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BZ_ERROR(...)        ::Log::GetClientLogger()->error(__VA_ARGS__)
#define BZ_FATAL(...)        ::Log::GetClientLogger()->critical(__VA_ARGS__)

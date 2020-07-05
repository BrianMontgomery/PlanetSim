#pragma once

#include "PSIM/Core/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


//logging class
class Log
{
public:
	static void init();
	static void shutdown();

	//getters
	inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_CoreLogger; }
	inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_ClientLogger; }

private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

// Core log macros
#define PSIM_CORE_TRACE(...)    ::Log::getCoreLogger()->trace(__VA_ARGS__)
#define PSIM_CORE_INFO(...)     ::Log::getCoreLogger()->info(__VA_ARGS__)
#define PSIM_CORE_WARN(...)     ::Log::getCoreLogger()->warn(__VA_ARGS__)
#define PSIM_CORE_ERROR(...)    ::Log::getCoreLogger()->error(__VA_ARGS__)
#define PSIM_CORE_FATAL(...)    ::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define PSIM_TRACE(...)			::Log::getClientLogger()->trace(__VA_ARGS__)
#define PSIM_INFO(...)			::Log::getClientLogger()->info(__VA_ARGS__)
#define PSIM_WARN(...)			::Log::getClientLogger()->warn(__VA_ARGS__)
#define PSIM_ERROR(...)			::Log::getClientLogger()->error(__VA_ARGS__)
#define PSIM_FATAL(...)			::Log::getClientLogger()->critical(__VA_ARGS__)
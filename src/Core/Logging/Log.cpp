#pragma once
#include "PSIMPCH.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void Log::init() {
	PSIM_PROFILE_FUNCTION();
	//sets the logging format pattern
	spdlog::set_pattern("%^[%T] %n: %v%$");

	//core logger setup
	s_CoreLogger = spdlog::stdout_color_mt("System");
	s_CoreLogger->set_level(spdlog::level::trace);

	//client logger setup
	s_ClientLogger = spdlog::stdout_color_mt("App");
	s_ClientLogger->set_level(spdlog::level::trace);
}

void Log::shutdown() {
	PSIM_PROFILE_FUNCTION();
	spdlog::shutdown();
}

//ptr for logger objects
std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
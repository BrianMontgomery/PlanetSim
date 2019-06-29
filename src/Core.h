#pragma once

#ifdef PSIM_PLATFORM_WINDOWS
#else
	#error Psim only supports Windows!
#endif

#ifdef PSIM_DEBUG
	#define PSIM_ENABLE_ASSERTS
	const bool enableValidationLayers = true;
#else
	const bool enableValidationLayers = false;
#endif

#ifdef PSIM_ENABLE_ASSERTS
	#define PSIM_ASSERT(x, ...) { if(!(x)) { PSIM_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define PSIM_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)
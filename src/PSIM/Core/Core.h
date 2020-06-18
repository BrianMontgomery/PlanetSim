#pragma once

#ifdef PSIM_PLATFORM_WINDOWS
#else
	#error Psim only supports Windows!
#endif

#ifdef PSIM_DEBUG
	#define PSIM_ENABLE_ASSERTS
	#define PSIM_PROFILE 1
	const bool enableValidationLayers = true;
#else
	const bool enableValidationLayers = false;
#endif

#ifdef PSIM_ENABLE_ASSERTS
	#define PSIM_ASSERT(x, ...) { if(!(x)) { PSIM_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define PSIM_ASSERT(x, ...) { if(!(x)) { PSIM_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#endif

#define BIT(x) (1 << x)

#define PSIM_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}
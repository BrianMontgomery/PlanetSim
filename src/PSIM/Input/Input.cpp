#include "PSIMPCH.h"
#include "PSIM/Input/Input.h"

#ifdef PSIM_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsInput.h"
#endif

Scope<Input> Input::s_Instance = Input::Create();

Scope<Input> Input::Create()
{
#ifdef PSIM_PLATFORM_WINDOWS
	return CreateScope<WindowsInput>();
#else
	PSIM_ASSERT(false, "Unknown platform!");
	return nullptr;
#endif
	}
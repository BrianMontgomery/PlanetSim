#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Core/Core.h"
#include "Core/Logging/Log.h"
#include "Core/Debug/Instrumentor.h"

#ifdef PSIM_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
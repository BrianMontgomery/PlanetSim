workspace "PlanetSim"
	architecture "x64"
	startproject "PlanetSim"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["glad"] = "vendor/glad/include"
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb"] = "vendor/stb"
IncludeDir["tinyObjLoader"] = "vendor/tinyObjLoader"

group "Dependencies"
	include "vendor/GLFW"
	include "vendor/glad"

group ""

project "PlanetSim"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputDir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

	pchheader "PSIMPCH.h"
	pchsource "src/PSIMPCH.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/PSIM/Shaders/**.spv",
		"vendor/glad/include/glad/**.h",
		"vendor/glad/include/KHR/**.h",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/stb/stb/**.h",
		"vendor/tinyObjLoader/tinyObjLoader/**.h"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"VULKAN_HPP_NO_EXCEPTIONS",
		"GLM_FORCE_RADIANS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		"GLM_ENABLE_EXPERIMENTAL",
		"TINYOBJLOADER_IMPLEMENTATION"
	}

	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.tinyObjLoader}",
		
		"C:/VulkanSDK/1.1.121.2/Include"
	}

	libdirs { 
				"C:/VulkanSDK/1.1.121.2/Lib",
				"vendor/glfw/bin/Debug-windows-x86_64/glfw"
	}

	links 
	{ 
		"GLFW",
		"glad",
		"vulkan-1.lib",
		"glfw.lib",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"PSIM_PLATFORM_WINDOWS",
			"VK_USE_PLATFORM_WIN32_KHR"
		}

	filter "configurations:Debug"
		defines "PSIM_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "PSIM_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "PSIM_DIST"
		runtime "Release"
		optimize "on"
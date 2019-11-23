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
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb"] = "vendor/stb"
IncludeDir["tinyObjLoader"] = "vendor/tinyObjLoader"

group "Dependencies"
	include "vendor/GLFW"

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
		"src/Shaders/**.spv",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/stb/stb/**.h",
		"vendor/tinyObjLoader/tinyObjLoader/**.h"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"STB_IMAGE_IMPLEMENTATION",
		"GLM_FORCE_RADIANS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		"GLM_ENABLE_EXPERIMENTAL",
		"TINYOBJLOADER_IMPLEMENTATION"
	}

	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.tinyObjLoader}",
		
		"C:/VulkanSDK/1.1.108.0/Include"
	}

	libdirs { 
				"C:/VulkanSDK/1.1.108.0/Lib",
				"vendor/glfw/bin/Debug-windows-x86_64/glfw"
	}

	links 
	{ 
		"GLFW",
		"vulkan-1.lib",
		"glfw.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"PSIM_PLATFORM_WINDOWS"
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
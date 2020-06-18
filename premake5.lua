workspace "PlanetSim"
	architecture "x64"
	startproject "PlanetSim"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	flags
	{
		"MultiProcessorCompile"
	}


outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb"] = "vendor/stb"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["tinyObjLoader"] = "vendor/tinyObjLoader"

group "Dependencies"
	include "vendor/GLFW"
	include "vendor/imgui"

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
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/stb/stb/**.h",
		--basic tiny obj loader
		"vendor/tinyObjLoader/tinyObjLoader/**.h",
		--optimized tiny obj loader
		"vendor/tinyObjLoader/experimental/**.h",
		"vendor/tinyObjLoader/experimental/**.hpp",

		"assets/shaders/**.spv",
		"assets/models/**.obj",
		"assets/textures/**.jpg"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"VULKAN_HPP_NO_EXCEPTIONS",
		"STB_IMAGE_IMPLEMENTATION",
		"GLM_FORCE_RADIANS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		"GLM_ENABLE_EXPERIMENTAL",
		--choose optimized or non-optimized tinyobj
		--"TINYOBJLOADER_IMPLEMENTATION"
		"TINYOBJ_LOADER_OPT_IMPLEMENTATION"
	}

	includedirs
	{
		"src",
		"src/Platform",
		"src/PSIM",
		"vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.ImGui}",
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
		"ImGui",
		"vulkan-1.lib",
		"glfw.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
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
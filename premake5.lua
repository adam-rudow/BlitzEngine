
workspace "BlitzEngine"
	architecture "x86"

	configurations
	{
		"Debug",
		"Release",
		"AssetProcessor",
	}

	---"Debug/Windows/x64"
outputDir = "%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}"

--[postbuildcommands
--{
--	("{COPY} %{cfg.buildtarget.relpath} ../Output-p/Binaries/" .. outputDir .. "/FeatureTestGame")
--}
--]

project "BlitzCommon"
	location "Engine/BlitzCommon"
	kind "StaticLib"
	language "C++"

	targetdir  ("Output-p/Binaries/" .. outputDir)-- .. "/%{prj.name}")
	objdir ("Output-p/Intermediate/" .. outputDir)-- .. "/%{prj.name}")

	files
	{
		"Engine/%{prj.name}/Source/Containers/**.h",
		"Engine/%{prj.name}/Source/Containers/**.cpp",
		"Engine/%{prj.name}/Source/Logging/**.h",
		"Engine/%{prj.name}/Source/Logging/**.cpp",
		"Engine/%{prj.name}/Source/Math/**.h",
		"Engine/%{prj.name}/Source/Math/**.cpp",
		"Engine/%{prj.name}/Source/Utils/**.h",
		"Engine/%{prj.name}/Source/Utils/**.cpp",
	}

	includedirs
	{
		"%{prj.name}",
		"External/spdlog/include",
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		--systemversion "8.1"

		defines
		{
			"_WINDLL",
			"_MATHDLL",
			"_COMMON_DLL",
		}

		

	filter "configurations:Debug"
		defines ""
		symbols "On"
		
	filter "configurations:Release"
		defines ""
		optimize "On"
		
	filter "configurations:AssetProcessor"
		defines ""
		symbols "On"
		optimize "On"



project "FeatureTestGame"
	location "Games/FeatureTestGame"
	kind "ConsoleApp"
	language "C++"

	--targetdir  ("Output-p/Binaries/" .. outputDir .. "/%{prj.name}")
	--objdir ("Output-p/Intermediate/" .. outputDir .. "/%{prj.name}")
	targetdir  ("Games/%{prj.name}/Output-p/Binaries/" .. outputDir)
	objdir ("Games/%{prj.name}/Output-p/Intermediate/" .. outputDir)

	files
	{
		"Game/%{prj.name}/Source/**.h",
		"Game/%{prj.name}/Source/**.cpp",
	}

	includedirs
	{
		"%{prj.name}",
		"External/spdlog/include",
		--"Engine\BlitzSystem",
		"Engine/BlitzCommon",
	}

	links
	{
		"BlitzCommon.lib",
		--"BlitzSystem",
		--"BlitzRenderer",
	}

	libdirs { "libs", "Output-p/Binaries/" .. outputDir }

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		--systemversion "8.1"

		defines
		{
			"_WINDLL",
			"_MATHDLL",
			"_COMMON_DLL",
		}		

	filter "configurations:Debug"
		defines ""
		symbols "On"
		
	filter "configurations:Release"
		defines ""
		optimize "On"
	
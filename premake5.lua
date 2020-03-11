
workspace "BlitzEngine"
	architecture "x86"

	configurations
	{
		"Debug",
		"Release",
		"AssetProcessor",
	}
	
	characterset ("MBCS")

	---"Debug/Windows-x64"
outputDir = "%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}"
d3d_SDK_includePath = "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/include"
d3d_SDK_libPath     = "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/%{cfg.architecture}"
fbx_SDK_includePath     = "C:/Program Files/Autodesk/FBX/FBX SDK/2018.1.1/include"
fbx_SDK_libPath_debug   = "C:/Program Files/Autodesk/FBX/FBX SDK/2018.1.1/lib/vs2015/%{cfg.architecture}/debug"
fbx_SDK_libPath_release = "C:/Program Files/Autodesk/FBX/FBX SDK/2018.1.1/lib/vs2015/%{cfg.architecture}/release"
windows_SDK_includePath =
{
	"C:/Program Files (x86)/Windows Kits/8.1/Include/um",
	"C:/Program Files (x86)/Windows Kits/8.1/Include/shared",
	"C:/Program Files (x86)/Windows Kits/8.1/Include/winrt"
}
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
		"Engine/%{prj.name}/Source",
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

		disablewarnings { "4996" }

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

		
-----------------------------------------------------------------
---------------        BlitzSystem      -------------------------
-----------------------------------------------------------------
project "BlitzSystem"
	location "Engine/BlitzSystem"
	kind "StaticLib"
	language "C++"

	targetdir  ("Output-p/Binaries/" .. outputDir)
	objdir ("Output-p/Intermediate/" .. outputDir)

	files
	{
		"Engine/%{prj.name}/Source/**.h",
		"Engine/%{prj.name}/Source/**.cpp",
		"Engine/%{prj.name}/Source/Components/**.h",
		"Engine/%{prj.name}/Source/Components/**.cpp",
		"Engine/%{prj.name}/Source/Game/**.h",
		"Engine/%{prj.name}/Source/Game/**.cpp",
		"Engine/%{prj.name}/Source/Rendering/**.h",
		"Engine/%{prj.name}/Source/Rendering/**.cpp",
	}

	includedirs
	{
		"Engine/%{prj.name}/Source",
		"Engine/BlitzCommon/Source",
		"External/spdlog/include",
	}
	
	links
	{
		"BlitzCommon.lib",
	}

	libdirs { "Output-p/Binaries/" .. outputDir }

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		--systemversion "8.1"

		defines
		{
			"_WINDLL",
			"_SYSTEMDLL",
		}

		disablewarnings { "4996" }

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

-----------------------------------------------------------------
---------------        BlitzRenderer      -------------------------
-----------------------------------------------------------------
project "BlitzRenderer"
	location "Engine/BlitzRenderer"
	kind "StaticLib"
	language "C++"

	targetdir  ("Output-p/Binaries/" .. outputDir)
	objdir ("Output-p/Intermediate/" .. outputDir)

	files
	{
		"Engine/%{prj.name}/Source/**.h",
		"Engine/%{prj.name}/Source/**.cpp",
		"Engine/%{prj.name}/Source/FW1FontWrapper/**.h",
		"Engine/%{prj.name}/Source/FW1FontWrapper/**.cpp",
	}

	includedirs
	{
		"Engine/%{prj.name}/Source",
		"Engine/BlitzCommon/Source",
		"Engine/BlitzSystem/Source",
		"External/spdlog/include",
		d3d_SDK_includePath,
		windows_SDK_includePath,
	}
	
	links
	{
		"BlitzCommon.lib",
		"BlitzSystem.lib",
	}
	
	libdirs
	{
		"Output-p/Binaries/" .. outputDir,	
		d3d_SDK_libPath,
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		--systemversion "8.1"

		defines
		{
			"_WINDLL",
			"_SYSTEMDLL",
		}

		disablewarnings { "4996" }

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

		
-----------------------------------------------------------------
---------------        AssetProcessor    -------------------------
-----------------------------------------------------------------
project "AssetProcessor"
	location "Engine/AssetProcessor"
	language "C++"

	targetdir  ("Output-p/Binaries/" .. outputDir)
	objdir ("Output-p/Intermediate/" .. outputDir)

	files
	{
		"Engine/%{prj.name}/Source/**.h",
		"Engine/%{prj.name}/Source/**.cpp",
	}

	includedirs
	{
		"Engine/%{prj.name}/Source",
		"External/spdlog/include",
		d3d_SDK_includePath,
		fbx_SDK_includePath,
		windows_SDK_includePath,
		"Engine/BlitzSystem",
		"Engine/BlitzCommon",
	}

	links
	{
		"BlitzCommon.lib",
		"BlitzSystem.lib",
		"BlitzRenderer.lib",
		"libfbxsdk.lib",
	}
	
	libdirs
	{
		"Output-p/Binaries/" .. outputDir,		
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		--systemversion "8.1"

		defines
		{
			"WIN32",
			"FBXSDK_SHARED",
			"_CONSOLE",
			
		}

		disablewarnings { "4996" }

	filter "configurations:Debug"
		kind "StaticLib"
		symbols "On"
		defines
		{
			"_DEBUG",
			"_ASSET_LOADER_DLL",
			"_COMMON_DLL",
			"_MATHDLL",
			"_RENDERERDLL",
			"_SYSTEMDLL",
		}
		libdirs
		{
			fbx_SDK_libPath_release		
		}
		
	filter "configurations:Release"
		kind "StaticLib"
		optimize "On"
		defines
		{
			"_ASSET_LOADER_DLL",
			"_COMMON_DLL",
			"_MATHDLL",
			"_RENDERERDLL",
			"_SYSTEMDLL",
		}
		libdirs
		{
			fbx_SDK_libPath_release		
		}
		
	filter "configurations:AssetProcessor"
		kind "ConsoleApp"
		symbols "On"
		optimize "On"
		defines
		{
			"_DEBUG",
			"_ASSET_PROCESSOR",
		}
		libdirs
		{
			fbx_SDK_libPath_debug	
		}


-----------------------------------------------------------------
-------------------         Games      --------------------------
-----------------------------------------------------------------
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
		"Games/%{prj.name}/Source",
		"External/spdlog/include",
		"Engine/BlitzSystem",
		"Engine/BlitzCommon",
	}

	links
	{
		"BlitzCommon.lib",
		"BlitzSystem.lib",
		"BlitzRenderer.lib",
	}

	libdirs { "Output-p/Binaries/" .. outputDir }
	
	disablewarnings { "4996" }

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
		
		
	
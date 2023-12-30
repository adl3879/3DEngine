local CoralDotNetPath = os.getenv("CORAL_DOTNET_PATH")

project "Coral.Native"
    language "C++"
    cppdialect "C++20"
    kind "StaticLib"
    staticruntime "Off"

    -- Can't specify 64-bit architecture in the workspace level since VS 2022 (see https://github.com/premake/premake-core/issues/1758)
    architecture "x86_64"

	targetdir("Build/%{cfg.buildcfg}-%{cfg.system}")
	objdir("Intermediates/%{cfg.buildcfg}-%{cfg.system}")

    pchheader "CoralPCH.hpp"
    pchsource "Source/CoralPCH.cpp"

    forceincludes { "CoralPCH.hpp" }

    filter { "action:xcode4" }
        pchheader "Source/CoralPCH.hpp"
    filter { }

    files {
        "Source/**.cpp",
        "Source/**.hpp",

        "Include/Coral/**.hpp",
    }

    includedirs { "Source/", "Include/Coral/" }
    externalincludedirs { "../NetCore/" }

    filter { "configurations:Debug" }
        runtime "Debug"
        symbols "On"

    filter { "configurations:Release" }
        runtime "Release"
        symbols "Off"
        optimize "On"

	filter { "system:windows" }
		defines { "CORAL_WINDOWS" }

	filter { "system:linux" }
		defines { "CORAL_LINUX" }

	filter { "system:macosx" }
		defines { "CORAL_MACOSX" }

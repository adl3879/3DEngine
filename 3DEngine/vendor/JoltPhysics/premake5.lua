project 'JoltPhysics'
	kind "StaticLib"
	staticruntime "off"
	language "C++"
	cppdialect "C++20"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
  objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

	defines {
		
	}

	includedirs {
		"./"
	}

	files {
		"Jolt/**.h",
		"Jolt/**.cpp",
	}

	filter "configurations:Debug"
		cppdialect "C++17"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		cppdialect "C++17"
		runtime "Release"
		optimize "on"
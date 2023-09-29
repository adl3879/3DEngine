project "ScriptCore"
	kind "SharedLib"
	language "C#" 
	dotnetframework "4.7.2"

	targetdir ("%{wks.location}/Sandbox/Resources/Scripts")
	objdir ("%{wks.location}/Sandbox/Resources/Scripts/Intermediate")

	files
	{
		"Source/**.cs",
		"Properties/**.cs"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "Default"

	filter "configurations:Release"
		runtime "Release"
		symbols "Default"

	filter "configurations:Dist"
		runtime "Release"
		symbols "Off"
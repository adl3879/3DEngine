include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "3DEngine"
	architecture "x86_64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	include "3DEngine/vendor/Box2D"
	include "3DEngine/vendor/GLFW"
	include "3DEngine/vendor/Glad"
	include "3DEngine/vendor/imgui"
	include "3DEngine/vendor/yaml-cpp"
	include "3DEngine/vendor/JoltPhysics"
group ""

group "Core"
	include "3DEngine"
	include "ScriptCore"
group ""

group "Misc"
	include "Sandbox"
group ""
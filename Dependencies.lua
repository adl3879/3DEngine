
-- 3DEngine Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/3DEngine/vendor/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/3DEngine/vendor/yaml-cpp/include"
IncludeDir["Box2D"] = "%{wks.location}/3DEngine/vendor/Box2D/include"
IncludeDir["GLFW"] = "%{wks.location}/3DEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/3DEngine/vendor/Glad/include"
IncludeDir["filewatch"] = "%{wks.location}/3DEngine/vendor/filewatch"
IncludeDir["ImGui"] = "%{wks.location}/3DEngine/vendor/ImGui"
IncludeDir["IconsFontAwesome5"] = "%{wks.location}/3DEngine/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/3DEngine/vendor/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/3DEngine/vendor/glm"
IncludeDir["JoltPhysics"] = "%{wks.location}/3DEngine/vendor/JoltPhysics"
IncludeDir["entt"] = "%{wks.location}/3DEngine/vendor/entt/include"
IncludeDir["shaderc"] = "%{wks.location}/3DEngine/vendor/shaderc/include"
IncludeDir["mono"] = "%{wks.location}/3DEngine/vendor/mono/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/3DEngine/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["assimp"] = "%{wks.location}/3DEngine/vendor/assimp/include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["mono"] = "%{wks.location}/3DEngine/vendor/mono/lib/%{cfg.buildcfg}"
LibraryDir["assimp"] = "%{wks.location}/3DEngine/vendor/assimp"

Library = {}
Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"
Library["assimp"] = "%{LibraryDir.assimp}/assimp-vc143-mt.lib"
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Tools.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt.lib"] = "Bcrypt.lib"
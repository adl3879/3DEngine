cmake_minimum_required(VERSION 3.17)

project(3DEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDebug)

# external libraries
add_subdirectory(vendor/glad)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/yaml-cpp)
add_subdirectory(vendor/JoltPhysics/Build)
add_subdirectory(vendor/Coral/Coral.Native)

target_compile_definitions(Jolt PRIVATE
        __STDC_WANT_SECURE_LIB__=1  # Set to 0 if you want to disable secure library features
)

file(GLOB_RECURSE SOURCES
        "src/*.cpp"
        "vendor/stb_image/stb_image.cpp"
        "vendor/ImGuizmo/ImGuizmo.cpp"
        "vendor/ImGuiColorTextEdit/TextEditor.cpp"
)
file(GLOB_RECURSE HEADERS
        "src/*.h"
        "vendor/stb_image/stb_image.h"
        "vendor/entt/include/entt.hpp"
        "vendor/ImGuizmo/ImGuizmo.h"
        "vendor/filewatch/FileWatch.h"
        "vendor/ImGuiColorTextEdit/TextEditor.h"
)

include_directories(
        "src/Renderer"
        "vendor"
        "vendor/glad/include"
        "vendor/glm"
        "vendor/spdlog/include"
        "vendor/assimp/include"
        "vendor/imgui"
        "vendor/entt/include"
        "vendor/yaml-cpp/include"
        "vendor/ImGuizmo"
        "vendor/JoltPhysics"
        "vendor/Coral/Coral.Native/Include"
)

set (INCLUDE_DIRS "")
foreach (_headerFile ${HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES INCLUDE_DIRS)

add_library(${PROJECT_NAME} STATIC ${SOURCES}
        src/Scripting/NetModules/SceneNetAPI.h
        src/Utils/CustomString.h
        src/Utils/String.cpp "src/Renderer/SkinnedModel.h" "src/Renderer/SkinnedModel.cpp" "src/Renderer/Mesh/Bone.h" "src/Renderer/Mesh/Bone.cpp" "src/Resource/Animator.h" "src/Resource/Animator.cpp" "src/Asset/MeshImporter.cpp" "src/Resource/ImportData.h" "src/Resource/ImportData.cpp")

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${INCLUDE_DIRS}
        glad glfw assimp imgui glm spdlog yaml-cpp
        Jolt
        Coral.Native
        PRIVATE
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        glm imgui spdlog yaml-cpp
        Jolt
        Coral.Native
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp/assimp-vc143-mt.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Coral/NetCore/7.0.7/nethost.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Coral/NetCore/7.0.7/libnethost.lib
        PRIVATE
        glad glfw
)
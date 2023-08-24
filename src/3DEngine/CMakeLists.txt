cmake_minimum_required(VERSION 3.17)

project(3DEngine)

set(CMAKE_CXX_STANDARD 20)

# external libraries
add_subdirectory(vendor/glad)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/assimp)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/yaml-cpp)
add_subdirectory(vendor/JoltPhysics/Build)

file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "vendor/stb_image/stb_image.cpp"
    "vendor/tinyfiledialogs/tinyfiledialogs.c"
    "vendor/ImGuizmo/ImGuizmo.cpp"
)
file(GLOB_RECURSE HEADERS
    "src/*.h"
    "vendor/stb_image/stb_image.h"
    "vendor/entt/include/entt.hpp"
    "vendor/sol3/sol/sol.hpp"
    "vendor/tinyfiledialogs/tinyfiledialogs.h"
    "vendor/ImGuizmo/ImGuizmo.h"
)

include_directories(
    "vendor/glad/include"
    "vendor/glm"
    "vendor/spdlog/include"
    "vendor/assimp/include"
    "vendor/imgui"
    "vendor/entt/include"
    "vendor/lua542/include"
    "vendor/sol3"
    "vendor/yaml-cpp/include"
    "vendor/tinyfiledialogs"
    "vendor/ImGuizmo"
    "vendor/JoltPhysics"
)

set (INCLUDE_DIRS "")
foreach (_headerFile ${HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES INCLUDE_DIRS)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${INCLUDE_DIRS}
        glad glfw assimp imgui glm spdlog yaml-cpp
        Jolt
    PRIVATE
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC
        glm assimp imgui spdlog yaml-cpp
        Jolt
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/lua542/liblua54.a
    PRIVATE 
        glad glfw
)

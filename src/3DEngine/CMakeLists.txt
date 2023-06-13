cmake_minimum_required(VERSION 3.17)

project(3DEngine)

set(CMAKE_CXX_STANDARD 20)

# external libraries
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/assimp)
add_subdirectory(vendor/imgui)

file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "vendor/glad/src/glad.c"
    "vendor/stb_image/stb_image.cpp"
)
file(GLOB_RECURSE HEADERS
    "src/*.h"
    "vendor/stb_image/stb_image.h"
    "vendor/entt/include/entt.hpp"
)

include_directories(
    "vendor/glm"
    "vendor/glad/include"
    "vendor/json"
    "vendor/assimp/include"
    "vendor/imgui"
    "vendor/entt/include"
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
        glfw assimp imgui glm
    PRIVATE
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC
        glm assimp imgui
    PRIVATE 
        glfw
)

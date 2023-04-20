cmake_minimum_required(VERSION 3.17)

project(3DEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PCH_FILE "../src/stdafx.h")

# exterrnal libraries
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "vendor/glad/src/glad.c"
    "vendor/stb_image/stb_image.cpp"
)

include_directories(
    "src/*"
    "vendor/stb_image"
    "vendor/glad/include"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        glm glfw
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE glm glfw
)

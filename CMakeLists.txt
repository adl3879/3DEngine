cmake_minimum_required(VERSION 3.17)

project(3DEngine)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "vendor/glad/src/glad.c"
    "vendor/stb_image/*.cpp"
    "vendor/glm/glm/*.cpp"
)

include_directories(
    "vendor/stb_image"
    "vendor/glm"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} GL dl glfw)

cmake_minimum_required(VERSION 3.17)

project(glad)

set(
        SOURCES
        "include/glad/glad.h"
        "src/glad.c"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "include")
cmake_minimum_required(VERSION 3.17)

project(3DEngineApplication VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

add_definitions(-D_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)

add_subdirectory(Sandbox)
add_subdirectory(3DEngine)
cmake_minimum_required(VERSION 3.17)

project(Sandbox)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/3DEngine/src "src/Panels")
target_link_libraries(${PROJECT_NAME}  3DEngine)
target_include_directories(${PROJECT_NAME}  PRIVATE 3DEngine)

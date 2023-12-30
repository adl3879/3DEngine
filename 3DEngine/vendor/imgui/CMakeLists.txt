cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

project(imgui)

set(SOURCES
        "imconfig.h"
        "imgui.h"
        "imgui.cpp"
        "imgui_draw.cpp"
        "imgui_internal.h"
        "imgui_widgets.cpp"
        "imstb_rectpack.h"
        "imstb_textedit.h"
        "imstb_truetype.h"
        "imgui_demo.cpp"
        "imgui_tables.cpp"

        "backends/imgui_impl_glfw.h"
        "backends/imgui_impl_glfw.cpp"
        "backends/imgui_impl_opengl3.h"
        "backends/imgui_impl_opengl3.cpp"

        "IconsFontAwesome5.h"
)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
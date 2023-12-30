cmake_minimum_required(VERSION 3.17)

project(Sandbox)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDebug)

file(GLOB_RECURSE SOURCES
        "src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES}
        src/Panels/InputMapPanel.h
        src/Panels/InputMapPanel.h)
# Set the output directory for the executable to the current binary directory (Sandbox)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/3DEngine/src
        "src/Panels"
)
target_link_libraries(${PROJECT_NAME} 3DEngine)
target_include_directories(${PROJECT_NAME} PRIVATE 3DEngine)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/3DEngine/vendor/assimp/assimp-vc143-mt.dll
        ${CMAKE_CURRENT_BINARY_DIR}/assimp-vc143-mt.dll)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/3DEngine/vendor/Coral/NetCore/7.0.7/nethost.dll
        ${CMAKE_CURRENT_BINARY_DIR}/nethost.dll)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/3DEngine/vendor/Coral/Coral.Managed/Coral.Managed.runtimeconfig.json
        ${CMAKE_CURRENT_BINARY_DIR}/Coral.Managed.runtimeconfig.json)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/3DEngine/vendor/Coral/Coral.Managed/Build/Debug-windows/Coral.Managed.dll
        ${CMAKE_CURRENT_BINARY_DIR}/Coral.Managed.dll)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/ScriptCore/ScriptCore/bin/Debug/net8.0/ScriptCore.dll
        ${CMAKE_CURRENT_BINARY_DIR}/ScriptCore.dll)

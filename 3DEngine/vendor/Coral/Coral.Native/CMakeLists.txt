# Set CoralDotNetPath from the environment variable
set(CoralDotNetPath $ENV{CORAL_DOTNET_PATH})

# Project configuration
project(Coral.Native LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Target configuration
add_library(Coral.Native STATIC)
target_compile_features(Coral.Native PRIVATE cxx_std_20)
set_target_properties(Coral.Native PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build/$<CONFIG>-$<PLATFORM_ID>"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build/$<CONFIG>-$<PLATFORM_ID>"
        OUTPUT_NAME_DEBUG "Coral.Native_Debug"
        OUTPUT_NAME_RELEASE "Coral.Native_Release"
)

# Can't specify 64-bit architecture in the workspace level since VS 2022
# https://github.com/premake/premake-core/issues/1758
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set_target_properties(Coral.Native PROPERTIES
            OUTPUT_NAME_DEBUG "Coral.Native_Debug_x64"
            OUTPUT_NAME_RELEASE "Coral.Native_Release_x64"
    )
endif()

# Source files
file(GLOB_RECURSE SOURCE_FILES "Source/*.cpp" "Source/*.hpp")
target_sources(Coral.Native PRIVATE ${SOURCE_FILES})

# Precompiled header
target_precompile_headers(Coral.Native PRIVATE "Source/CoralPCH.hpp")

# Include directories
target_include_directories(Coral.Native PRIVATE "Source/" "Include/Coral/")
include_directories("Source/" "Include/Coral/")

# External include directories
include_directories("../NetCore/7.0.7/")

# if windows
if (WIN32)
    target_compile_definitions(
            Coral.Native PRIVATE
            "CORAL_WINDOWS"
    )
endif()
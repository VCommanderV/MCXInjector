cmake_minimum_required(VERSION 3.20)
project(MCXInjector LANGUAGES CXX)

if(NOT CMAKE_GENERATOR_PLATFORM)
    set(CMAKE_GENERATOR_PLATFORM Win32 CACHE STRING "" FORCE)
endif()

if(NOT MSVC)
    message(FATAL_ERROR "This project requires MSVC.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(/W3 /permissive- /Zc:__cplusplus)

set(DETOURS_ROOT ${CMAKE_SOURCE_DIR}/external/detours)
set(ABSEIL_ROOT ${CMAKE_SOURCE_DIR}/external/abseil)

if(NOT EXISTS "${DETOURS_ROOT}/src/detours.h")
    message(FATAL_ERROR "Detours not found. Please run `git submodule update --init --recursive`.")
endif()

add_library(Detours STATIC IMPORTED)
set_target_properties(Detours PROPERTIES
    IMPORTED_LOCATION "${DETOURS_ROOT}/lib.X86/detours.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${DETOURS_ROOT}/src"
)

add_subdirectory(${ABSEIL_ROOT})
add_subdirectory(MCXInjector)
add_subdirectory(MCXLauncher)
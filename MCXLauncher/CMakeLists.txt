cmake_minimum_required(VERSION 3.20)

add_executable(MCXLauncher
    mcx_launcher.cpp
)

target_include_directories(MCXLauncher PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(MCXLauncher PRIVATE
    MCXInjector
    Detours
    absl::log
    absl::log_initialize
    absl::flags_parse
)

target_compile_definitions(MCXLauncher PRIVATE
    WIN32
    _CONSOLE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

set_target_properties(MCXLauncher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
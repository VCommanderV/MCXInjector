cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE MCXINJECTOR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE MCXINJECTOR_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(MCXInjector SHARED
    ${MCXINJECTOR_SOURCES}
    ${MCXINJECTOR_HEADERS}
)

target_include_directories(MCXInjector PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(MCXInjector PRIVATE 
    Detours
    absl::log
    absl::log_initialize
)

target_compile_definitions(MCXInjector PRIVATE
    WIN32
    _CONSOLE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR
)

set_target_properties(MCXInjector PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
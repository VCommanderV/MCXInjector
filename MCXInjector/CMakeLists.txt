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

set(LOG_SUBSYSTEMS
    HEAP
    BASEFILE
    FASTFILE
    FITINIFILE
    FILETRAFF
    AOBJECT
)

foreach(subsys IN LISTS LOG_SUBSYSTEMS)
    option(MCXI_ENABLE_${subsys}_LOGGING "Enable ${subsys} logging" OFF)
    
    if(MCXI_ENABLE_${subsys}_LOGGING)
        target_compile_definitions(MCXInjector PRIVATE ENABLE_LOG_${subsys}=1)
    endif()
endforeach()

option(MCXI_WAIT_FOR_DEBUGGER "Wait for debugger to be attached" OFF)

if(MCXI_WAIT_FOR_DEBUGGER)
    target_compile_definitions(MCXInjector PRIVATE WAIT_FOR_DEBUGGER)
endif()

set_target_properties(MCXInjector PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
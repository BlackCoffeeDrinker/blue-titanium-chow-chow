#######################################################################################################################
## Compiler identification
#######################################################################################################################
include(CheckSymbolExists)
if (WIN32)
    check_symbol_exists("_M_IX86" "" ARCH_X86)
    check_symbol_exists("_M_AMD64" "" ARCH_X64)
    check_symbol_exists("_M_ARM" "" ARCH_ARM)
    check_symbol_exists("_M_ARM64" "" ARCH_ARM64)
else ()
    check_symbol_exists("__i386__" "" ARCH_X86)
    check_symbol_exists("__x86_64__" "" ARCH_X64)
    check_symbol_exists("__arm__" "" ARCH_ARM)
    check_symbol_exists("__aarch64__" "" ARCH_ARM64)
endif ()

if (ARCH_X64)
    set(TARGET_CPU "x64")
elseif (ARCH_X86)
    set(TARGET_CPU "x86")
elseif (ARCH_ARM64)
    set(TARGET_CPU "arm64")
elseif (ARCH_ARM)
    set(TARGET_CPU "arm")
else ()
    message(FATAL_ERROR "Unknown target: ${CMAKE_SYSTEM_PROCESSOR}")
endif ()

#######################################################################################################################
## SDL2
#######################################################################################################################
set(SDL_LIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/SDL2-2.0.16)
add_library(SDL SHARED IMPORTED GLOBAL)
target_include_directories(SDL INTERFACE ${SDL_LIB_ROOT}/include)
set_target_properties(SDL PROPERTIES
        IMPORTED_IMPLIB "${SDL_LIB_ROOT}/lib/${TARGET_CPU}/SDL2.lib"
        IMPORTED_LOCATION "${SDL_LIB_ROOT}/lib/${TARGET_CPU}/SDL2.dll"
        )

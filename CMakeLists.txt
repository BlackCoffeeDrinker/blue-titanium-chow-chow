cmake_minimum_required(VERSION 3.15)

# 
# This is an amalgamation of 3 projects:
# Level 0: Engine
# Level 1: Game-Specific Engine extends
# Level 2: Platform-Specific
#

set(MODERN_CMAKE_BUILD_TESTING On)
set(BUILD_TESTING On)

#######################################################################################################################
## Level 0: Engine (Should be distributed as a precompiled static lib)
#######################################################################################################################
add_subdirectory(Engine)

#######################################################################################################################
## Level 1: Game (Supplied by the end-game developers)
#######################################################################################################################
add_subdirectory(Game_Light)

#######################################################################################################################
## Level 2: Platform specific
#######################################################################################################################
if (DJGPP)
    message(STATUS "Building for MS-DOS Protected mode")
    add_subdirectory(Platform_DOS)
else()
    message(STATUS "Building for the SDL platform")
    add_subdirectory(Platform_SDL)
endif ()

# Keep CMake happy
project(Project)

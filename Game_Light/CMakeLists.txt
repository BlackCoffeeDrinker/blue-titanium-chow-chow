cmake_minimum_required(VERSION 3.15)

#######################################################################################################################
## Main Project
#######################################################################################################################
PROJECT(GameBlueTitanium
	VERSION 1.0.0
	DESCRIPTION "A small top-down ARPG, built on Engine01")

# Enable position-independent code globally. This is needed because
# some library targets are OBJECT libraries.
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

# We are targeting C++17
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# Always enable C++
ENABLE_LANGUAGE(CXX)

#######################################################################################################################
## CMake Module configuration
#######################################################################################################################
include(ExternalProject)
include(FetchContent)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
        ${PROJECT_SOURCE_DIR}/CMakeModules)

#######################################################################################################################
## Useful stuff to set only if this is the main project and not included through add_subdirectory
#######################################################################################################################
IF (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	# Make sure cmake uses -std=c++ and not -std=g++
	set(CMAKE_CXX_EXTENSIONS OFF)

	# Folders in IDEs
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)

	include(CTest)

	FIND_PACKAGE(Doxygen)
	if (Doxygen_FOUND)
		add_subdirectory(docs)
	else()
		message(STATUS "Doxygen not found, not building docs")
	endif()
endif()

#######################################################################################################################
## Main Game target
#######################################################################################################################
add_library(GameBlueTitanium STATIC)
target_link_libraries(GameBlueTitanium PUBLIC Engine00)

add_subdirectory(src)


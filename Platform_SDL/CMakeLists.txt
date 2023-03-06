
project(PlatformWindows
        VERSION 1.0.0
        LANGUAGES C CXX)

# We are targeting C++17
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(vendor)

add_executable(Game)
target_link_libraries(Game Engine00 SDL2::SDL2-static GameBlueTitanium)

add_subdirectory(src)

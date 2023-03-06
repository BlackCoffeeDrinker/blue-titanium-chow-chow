
project(PlatformDOS
        VERSION 1.0.0
        LANGUAGES C CXX)

add_executable(Game)
target_link_libraries(Game Engine00 GameBlueTitanium)
add_subdirectory(src)

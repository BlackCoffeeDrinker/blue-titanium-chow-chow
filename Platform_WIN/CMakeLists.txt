
project(PlatformWindows
        VERSION 1.0.0
        LANGUAGES C CXX)

add_subdirectory(vendor)

add_executable(Game WIN32)
get_target_property(SDL_DLL SDL IMPORTED_LOCATION)
add_custom_command(
        TARGET Game
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL_DLL} $<TARGET_FILE_DIR:Game>
)

target_link_libraries(Game Engine00 SDL GameBlueTitanium)

add_subdirectory(src)

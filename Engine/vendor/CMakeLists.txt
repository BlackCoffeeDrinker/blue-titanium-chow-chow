
#######################################################################################################################
## LodePNG
#######################################################################################################################
add_library(lodepng OBJECT lodepng/lodepng.cpp lodepng/lodepng.h)
target_include_directories(lodepng SYSTEM INTERFACE lodepng)
source_group("Vendor: Lodepng" FILES lodepng/lodepng.cpp lodepng/lodepng.h)

#######################################################################################################################
## Magic Enum
#######################################################################################################################
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum SYSTEM INTERFACE magic_enum/include)

#######################################################################################################################
## INI
#######################################################################################################################
add_library(ini OBJECT inih/ini.c inih/ini.h)
target_include_directories(ini SYSTEM INTERFACE inih)

#######################################################################################################################
## LUA
#######################################################################################################################
add_library(Lua STATIC
        lua/lapi.c
        lua/lcode.c
        lua/lctype.c
        lua/ldebug.c
        lua/ldo.c
        lua/ldump.c
        lua/lfunc.c
        lua/lgc.c
        lua/llex.c
        lua/lmem.c
        lua/lobject.c
        lua/lopcodes.c
        lua/lparser.c
        lua/lstate.c
        lua/lstring.c
        lua/ltable.c
        lua/ltm.c
        lua/lundump.c
        lua/lvm.c
        lua/lzio.c

        lua/lauxlib.c
        lua/lbaselib.c
        lua/lcorolib.c
        lua/ldblib.c
        lua/liolib.c
        lua/lmathlib.c
        lua/loadlib.c
        lua/loslib.c
        lua/lstrlib.c
        lua/ltablib.c
        lua/lutf8lib.c
        lua/linit.c
        )
target_compile_definitions(Lua PUBLIC LUA_COMPAT_5_3)
target_compile_definitions(Lua PUBLIC LUA_32BITS=1)
target_include_directories(Lua PUBLIC lua)



#######################################################################################################################
## nlohmann/json
#######################################################################################################################
set(JSON_MultipleHeaders OFF)
add_subdirectory(json)

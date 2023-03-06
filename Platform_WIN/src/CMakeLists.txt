
set(WIN32_SRC
        main.cpp
        string_2_wstring.hpp
        StdFile.cpp StdFile.hpp

        )

target_sources(Game PRIVATE ${WIN32_SRC})

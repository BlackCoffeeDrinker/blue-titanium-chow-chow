
set(GAME_SRC
        LightGame.cpp
        LightGameEngine.cpp
        LightGameEngine.hpp
        )


target_sources(GameBlueTitanium PRIVATE ${GAME_SRC})
source_group("Game specific logic" FILES ${GAME_SRC})

add_library(app
    src/Application.cpp
)

target_link_libraries(app
    PUBLIC
        engine
        game
        platform
)

target_include_directories(app
    PUBLIC
        include
)

add_executable(MacriBattleStars
    src/main.cpp
)

if(WIN32)
    copy_sdl_dlls(MacriBattleStars)
endif()

target_link_libraries(MacriBattleStars
    PRIVATE
        app
        engine
        game
)

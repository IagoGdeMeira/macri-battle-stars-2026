add_executable(MacriBattleStars
    main.cpp
)

if(WIN32)
    copy_sdl_dlls(MacriBattleStars)
endif()

target_link_libraries(MacriBattleStars
    PRIVATE
        game
        platform
)

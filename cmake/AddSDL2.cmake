# ==========================================================
# AddSDL2.cmake
# Local configuration of the SDL2 libraries for Windows
# ==========================================================

if(NOT WIN32)
    return()
endif()

message(STATUS "Using local SDL2 from /external directory")

# ----------------------------------------------------------
# Detect architecture (x86 or x64)
# ----------------------------------------------------------

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SDL_ARCH x64)
else()
    set(SDL_ARCH x86)
endif()

message(STATUS "Detected architecture: ${SDL_ARCH}")

# ----------------------------------------------------------
# Base paths
# ----------------------------------------------------------

set(SDL2_ROOT        ${PROJECT_SOURCE_DIR}/external/SDL2)
set(SDL2_IMAGE_ROOT  ${PROJECT_SOURCE_DIR}/external/SDL2_image)
set(SDL2_TTF_ROOT    ${PROJECT_SOURCE_DIR}/external/SDL2_ttf)
set(SDL2_MIXER_ROOT  ${PROJECT_SOURCE_DIR}/external/SDL2_mixer)

# ----------------------------------------------------------
# Helper function to create imported target
# ----------------------------------------------------------

function(import_sdl_library TARGET_NAME ROOT LIB_NAME DLL_NAME)

    add_library(${TARGET_NAME} SHARED IMPORTED)

    set_target_properties(${TARGET_NAME} PROPERTIES
        IMPORTED_IMPLIB ${ROOT}/lib/${SDL_ARCH}/${LIB_NAME}
        IMPORTED_LOCATION ${ROOT}/bin/${SDL_ARCH}/${DLL_NAME}
        INTERFACE_INCLUDE_DIRECTORIES ${ROOT}/include
    )

endfunction()

# ----------------------------------------------------------
# Import libraries
# ----------------------------------------------------------

import_sdl_library(SDL2           ${SDL2_ROOT}       SDL2.lib        SDL2.dll)
import_sdl_library(SDL2_image     ${SDL2_IMAGE_ROOT} SDL2_image.lib  SDL2_image.dll)
import_sdl_library(SDL2_ttf       ${SDL2_TTF_ROOT}   SDL2_ttf.lib    SDL2_ttf.dll)
import_sdl_library(SDL2_mixer     ${SDL2_MIXER_ROOT} SDL2_mixer.lib  SDL2_mixer.dll)

# ----------------------------------------------------------
# Function to copy DLLs after build
# ----------------------------------------------------------

function(copy_sdl_dlls TARGET)

    set(ALL_SDL_DLLS)

    foreach(ROOT
        ${SDL2_ROOT}
        ${SDL2_IMAGE_ROOT}
        ${SDL2_TTF_ROOT}
        ${SDL2_MIXER_ROOT}
    )
        file(GLOB ROOT_DLLS
            ${ROOT}/bin/${SDL_ARCH}/*.dll
        )

        list(APPEND ALL_SDL_DLLS ${ROOT_DLLS})
    endforeach()

    if(ALL_SDL_DLLS)
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${ALL_SDL_DLLS}
                $<TARGET_FILE_DIR:${TARGET}>
        )
    endif()

endfunction()

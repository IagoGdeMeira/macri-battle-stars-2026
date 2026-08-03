cmake_minimum_required(VERSION 3.17)

project(MacriBattleStars
    VERSION 0.1.0
    DESCRIPTION "2D Fighting Game"
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# ===============================
# 🔧 Global Configuration
# ===============================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===============================
# ⚙️ Compiler Configuration
# ===============================

if(MSVC)
    add_compile_options(/W4 /permissive- /FS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ===============================
# 📦 SDL2 Platform Configuration
# ===============================

if(WIN32)
    
    include(AddSDL2)

elseif(UNIX)

    message(STATUS "Configuring SDL2 for Linux (system installed)")

    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_mixer REQUIRED)

endif()

# ===============================
# 📂 Subdirectories
# ===============================

add_subdirectory(src/app)
add_subdirectory(src/domain)
add_subdirectory(src/engine)
add_subdirectory(src/game)
add_subdirectory(src/platform)

include(CTest)
add_subdirectory(tests)

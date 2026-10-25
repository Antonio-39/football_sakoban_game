cmake_minimum_required(VERSION 3.21)
project(Sokoban LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ---------------------------------------------------------------------------
# Local library installs (override with -DSDL3_ROOT_DIR=... / -DSDL3_IMAGE_ROOT_DIR=...)
# ---------------------------------------------------------------------------
set(SDL3_ROOT_DIR "C:/Users/anton/Desktop/randomProjects/sdl_solution/SDL3-3.4.2"
        CACHE PATH "Folder where SDL3 is installed")
set(SDL3_IMAGE_ROOT_DIR "C:/Users/anton/Desktop/randomProjects/sdl_solution/SDL3_image-3.4.2"
        CACHE PATH "Folder where SDL3_image is installed")
set(SDL3_TTF_ROOT_DIR "C:/Users/anton/Desktop/randomProjects/sdl_solution/SDL3_ttf-3.2.2"
        CACHE PATH "Folder where SDL3_ttf is installed")

list(APPEND CMAKE_PREFIX_PATH
        "${SDL3_ROOT_DIR}"
        "${SDL3_ROOT_DIR}/x86_64-w64-mingw32"
        "${SDL3_ROOT_DIR}/i686-w64-mingw32"
        "${SDL3_IMAGE_ROOT_DIR}"
        "${SDL3_IMAGE_ROOT_DIR}/x86_64-w64-mingw32"
        "${SDL3_IMAGE_ROOT_DIR}/i686-w64-mingw32"
        "${SDL3_TTF_ROOT_DIR}"
        "${SDL3_TTF_ROOT_DIR}/x86_64-w64-mingw32"
        "${SDL3_TTF_ROOT_DIR}/i686-w64-mingw32"
)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
add_executable(ftsako
        src/main.cpp
        src/SDLState.h
        src/types.h
        src/Level.cpp
        src/Level.h
        src/Game.h
        src/Resources.cpp
        src/Resources.h
        src/Renderer.cpp
        src/Renderer.h
        # src/game.cpp
        # src/level.cpp
)

target_include_directories(ftsako PRIVATE src)

target_link_libraries(ftsako PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
)

if(MSVC)
    target_compile_options(ftsako PRIVATE /W4)
else()
    target_compile_options(ftsako PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------------------------------------------
# Post-build: copy assets and required DLLs next to the executable
# ---------------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET ftsako POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:ftsako>/assets"
            COMMENT "Copying assets to build directory"
    )
endif()

if(WIN32)
    add_custom_command(TARGET ftsako POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:ftsako>
            $<TARGET_FILE_DIR:ftsako>
            COMMAND_EXPAND_LISTS
    )
endif()
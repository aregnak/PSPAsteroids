cmake_minimum_required(VERSION 3.31)

project(Asteroids)

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)
set(GRAPHICS_DIR ${CMAKE_SOURCE_DIR}/graphics)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(ENTITIES_DIR ${CMAKE_SOURCE_DIR}/src/entities)
set(GAME_DIR ${CMAKE_SOURCE_DIR}/src/game)
set(AUDIO_DIR ${CMAKE_SOURCE_DIR}/src/audio)
set(TEXT_DIR ${CMAKE_SOURCE_DIR}/src/text)
set(INPUT_DIR ${CMAKE_SOURCE_DIR}/src/input)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
    ${COMMON_DIR}
    ${GRAPHICS_DIR}
    ${ENTITIES_DIR}
    ${GAME_DIR}
    ${AUDIO_DIR}
    ${TEXT_DIR}
    ${INPUT_DIR}
)

set(SOURCES
    ${SRC_DIR}/main.c
    ${COMMON_DIR}/callback.c
    ${COMMON_DIR}/extra.c
    ${GRAPHICS_DIR}/gu.c
    ${GAME_DIR}/game.c
    ${ENTITIES_DIR}/heart.c
    ${ENTITIES_DIR}/triangle.c
    ${ENTITIES_DIR}/bullet.c
    ${ENTITIES_DIR}/asteroid.c
    ${AUDIO_DIR}/sound.c
    ${TEXT_DIR}/text.c
    ${INPUT_DIR}/input.c
)

add_executable(${PROJECT_NAME} ${SOURCES})
add_compile_options(-Wall -O2)

#set_target_properties(${PROJECT_NAME} PROPERTIES
#    COMPILE_FLAGS "-Wall -O0"
#    LINK_FLAGS ""
#)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pspgu
    pspgum
    pspge
    pspdisplay
    pspdebug
    pspctrl
    pspaudio
    pspaudiolib
    psputility
)

# Create an EBOOT.PBP file
create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH "${CMAKE_SOURCE_DIR}/assets/icon0.png"
    BACKGROUND_PATH "${CMAKE_SOURCE_DIR}/assets/pic0.png"
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 01.00
)

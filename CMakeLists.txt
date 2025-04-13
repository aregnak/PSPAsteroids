cmake_minimum_required(VERSION 3.31)

project(Asteroids)

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)
set(GRAPHICS_DIR ${CMAKE_SOURCE_DIR}/graphics)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(ENTITIES_DIR ${CMAKE_SOURCE_DIR}/src/entities)
#set(GAME_DIR ${CMAKE_SOURCE_DIR}/src/game)

include_directories(
    ${COMMON_DIR}
    ${GRAPHICS_DIR}
    ${ENTITIES_DIR}
    #${GAME_DIR}
)

set(SOURCES
    ${SRC_DIR}/main.c
    ${COMMON_DIR}/callback.c
    ${GRAPHICS_DIR}/gu.c
    #${GAME_DIR}/game.c
    ${ENTITIES_DIR}/heart.c
    ${ENTITIES_DIR}/triangle.c

)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    pspgu
    pspge
    pspdisplay
    pspdebug
    pspctrl
)

# Create an EBOOT.PBP file
create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 01.00
)

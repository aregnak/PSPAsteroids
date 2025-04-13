cmake_minimum_required(VERSION 3.31)

project(Asteroids)

include_directories(
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/graphics
	${CMAKE_SOURCE_DIR}/src
)

set(SOURCES
	main.c
	${CMAKE_SOURCE_DIR}/common/callback.c
	${CMAKE_SOURCE_DIR}/graphics/gu.c
	${CMAKE_SOURCE_DIR}/src/heart.c
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

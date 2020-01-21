cmake_minimum_required(VERSION 3.1)

# custom variables
set(HL_MOD_NAME mod)
set(HL_EXE_PATH "E:/Program Files/hl")
set(HL_INCLUDE_PATH ${HL_EXE_PATH}/include)

# MSVC: msbuild mod.sln /p:Configuration=Release
project(${HL_MOD_NAME} C)


# cmake variables
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG  ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_SOURCE_DIR})

# lib path
link_directories(
	${HL_EXE_PATH}
)
# include
include_directories(
	${HL_INCLUDE_PATH}
)

file(GLOB SRC
	"./mod/mod.c"
)

add_library(${HL_MOD_NAME} MODULE ${SRC})

target_link_libraries(${HL_MOD_NAME}
	libhl
)

set_target_properties(${HL_MOD_NAME}
	PROPERTIES
	OUTPUT_NAME ${HL_MOD_NAME}
	SUFFIX .hdll
)

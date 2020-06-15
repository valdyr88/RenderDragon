# Emscripten RenderDragon project setup

cmake_minimum_required(VERSION 3.4.1)
set(EMSCRIPTEN_PATH C:/WebAssembly/emsdk)
set(EMSCRIPTEN_CMAKE_PATH ${EMSCRIPTEN_PATH}/cmake/Modules/Platform/Emscripten.cmake)
set(EMSCRIPTEN_INCLUDE_DIR ${EMSCRIPTEN_PATH}/upstream/emscripten/system/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fexceptions")

set(CMAKE_EXECUTABLE_SUFFIX ".html")

set(MAIN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/emsc_out)

add_definitions(-D_DEBUG)

include(${CMAKE_SOURCE_DIR}/../graphics/graphics.cmake)
include(${CMAKE_SOURCE_DIR}/../graphics/utils/spirv-cross/spirv_cross.cmake)

set(CPP_ROOT ${CMAKE_SOURCE_DIR}/)
set(SOURCES ${CPP_ROOT}/)

file(GLOB_RECURSE CPP_HEADERS ${MAIN_SOURCE_DIR}/*.h)
file(GLOB_RECURSE CPP_SOURCES ${MAIN_SOURCE_DIR}/main_emscripten.cpp)

add_executable(
    render-dragon-test
    ${EMSCRIPTEN_INCLUDE_DIR}
    ${CPP_HEADERS}
    ${CPP_SOURCES}
	${GRAPHICS_SOURCE_FILES}
	${SPIRV_CROSS_SOURCE_FILES}
)

set_target_properties(
    render-dragon-test
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/emsc_out/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/emsc_out/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/emsc_out/bin"
    LINK_FLAGS
    "-O2 -s USE_SDL=2 -s USE_WEBGL2=1 -s MAX_WEBGL_VERSION=2 -s FORCE_FILESYSTEM=1 -s ASSERTIONS=1"
)
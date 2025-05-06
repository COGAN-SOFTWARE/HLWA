cmake_minimum_required(VERSION 3.20)

project(HLWR LANGUAGES CXX)

set(CMAKE_GENERATOR_PLATFORM x64)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/MP)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wno-unused-parameter)
endif()


set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/HLWR)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/HLWR)

file(GLOB_RECURSE HLWR_SOURCES CONFIGURE_DEPENDS HLWR/*.cpp HLWR/*.hpp)

add_library(HLWR STATIC ${HLWR_SOURCES})

target_compile_definitions(HLWR PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(HLWR PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
)


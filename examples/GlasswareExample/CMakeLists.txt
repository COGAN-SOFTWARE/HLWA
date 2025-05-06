cmake_minimum_required(VERSION 3.20)

project(GlasswareExample LANGUAGES CXX)

set(CMAKE_GENERATOR_PLATFORM x64)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/GlasswareExample)

include_directories("${CMAKE_SOURCE_DIR}/HLWA/include")

if (MSVC)
    add_compile_options(/MP)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wno-unused-parameter)
endif()

file(GLOB_RECURSE GWEX_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_executable(GlasswareExample ${GWEX_SOURCES})

target_link_libraries(GlasswareExample PRIVATE HLWA)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories("${CMAKE_SOURCE_DIR}/build/bin/HLWA/Debug-windows-x86_64")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    link_directories("${CMAKE_SOURCE_DIR}/build/bin/HLWA/Release-windows-x86_64")
endif()

target_compile_definitions(GlasswareExample PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(GlasswareExample PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
)
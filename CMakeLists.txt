cmake_minimum_required(VERSION 3.20)

project(HLWA LANGUAGES CXX)

set(CMAKE_GENERATOR_PLATFORM x64)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/HLWA)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/HLWA)

if (MSVC)
    add_compile_options(/MP)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wno-unused-parameter)
endif()

file(GLOB_RECURSE HLWA_SOURCES CONFIGURE_DEPENDS HLWA/*.cpp HLWA/*.hpp)

add_library(HLWA STATIC ${HLWA_SOURCES})

target_compile_definitions(HLWA PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(HLWA PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
)

target_compile_definitions(HLWA PRIVATE CS_HLWA_E_ALL)

add_subdirectory(examples/GlasswareExample)
add_subdirectory(examples/JumplistExample)
add_subdirectory(examples/RegistryExample)
add_subdirectory(examples/TaskbarExample)
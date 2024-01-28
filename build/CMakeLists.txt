cmake_minimum_required(VERSION 3.10)

project(RayTracer VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_compile_options(-O3 -Wall -pedantic)

if(WIN32)
    add_compile_options(-Wno-pedantic-ms-format)
endif()

file(GLOB_RECURSE SOURCES "../ray_tracer_threaded.c" "../utilities/*.c" "../utilities/geometric/*.c")
add_executable(ray_tracer_threaded ${SOURCES})

if(NOT WIN32)
    target_link_libraries(ray_tracer_threaded pthread)
endif()

set_target_properties(ray_tracer_threaded PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out")
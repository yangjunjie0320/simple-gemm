cmake_minimum_required(VERSION 3.10)
project(sgemm)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CHECK "Check the correctness" ON)
add_compile_options(-DCHECK=${CHECK})

find_package(Eigen3 REQUIRED)
if(EIGEN3_FOUND)
    include_directories(${EIGEN3_INCLUDE_DIRS})
    message(STATUS "Eigen3 found: ${EIGEN3_INCLUDE_DIRS}")
    message(STATUS "Eigen3 version: ${EIGEN3_VERSION}")
endif()

set(KEY "sgemm")
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/${KEY}-*.c")
foreach(f IN LISTS SOURCES)
    get_filename_component(basename ${f} NAME_WE)
    message(STATUS "preparing ${basename}")

    add_executable(main-${basename}-o0
        src/main.cxx src/${basename}.c
    )
    
    add_executable(main-${basename}-o3
        src/main.cxx src/${basename}.c
    )

    target_compile_options(main-${basename}-o0 PRIVATE -O0)
    target_compile_options(main-${basename}-o3 PRIVATE -march=native -O3 -Ofast)
endforeach()

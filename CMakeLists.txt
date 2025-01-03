cmake_minimum_required(VERSION 3.10)
project(sgemm)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(NN "Number of running threads" 1)
option(CHECK_CORRECTNESS "Check correctness of the implementation" OFF)
option(BLOCK_SIZE "Block size for the implementation" 64)

add_compile_options(-DNN=${NN})
add_compile_options(-DBLOCK_SIZE=${BLOCK_SIZE})
add_compile_options(-DCHECK_CORRECTNESS=${CHECK_CORRECTNESS})

# download eigen
include(FetchContent)
set(EIGEN_GIT https://gitlab.com/libeigen/eigen.git)  
set(EIGEN_VERSION 3.4.0)

# do not compile eigen
FetchContent_Declare(
    eigen
    GIT_REPOSITORY ${EIGEN_GIT}
    GIT_TAG ${EIGEN_VERSION}
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

FetchContent_MakeAvailable(eigen)
include_directories(${eigen_SOURCE_DIR})

file(GLOB SGEMM_SOURCES "${CMAKE_SOURCE_DIR}/src/sgemm-*.c")
foreach(f IN LISTS SGEMM_SOURCES)
    get_filename_component(sgemm_basename ${f} NAME_WE)
    string(REPLACE "sgemm-" "" v ${sgemm_basename})

    message(STATUS "preparing sgemm-${v}")

    add_executable(main-${v}-o0
        src/main.cxx src/sgemm-${v}.c
    )
    add_dependencies(main-${v}-o0 eigen)

    add_executable(main-${v}-o3
        src/main.cxx src/sgemm-${v}.c
    )
    add_dependencies(main-${v}-o3 eigen)

    target_compile_options(main-${v}-o0 PRIVATE -O0)
    target_compile_options(main-${v}-o3 PRIVATE -march=native -O3 -Ofast -funroll-loops)
endforeach()

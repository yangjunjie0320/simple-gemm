cmake_minimum_required(VERSION 3.10)
project(sgemm)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# download eigen
include(ExternalProject)
set(EIGEN_GIT https://gitlab.com/libeigen/eigen.git)  
set(EIGEN_VERSION 3.4.0)
ExternalProject_Add(
    eigen
    GIT_REPOSITORY ${EIGEN_GIT}
    GIT_TAG ${EIGEN_VERSION}
    PREFIX ${PROJECT_BINARY_DIR}/deps
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Get_Property(eigen SOURCE_DIR)
include_directories(${SOURCE_DIR})

find_package(BLAS REQUIRED)
include_directories(${BLAS_INCLUDE_DIRS})

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

    target_link_libraries(main-${v}-o0
        PRIVATE
            ${BLAS_LIBRARIES}
    )

    target_link_libraries(main-${v}-o3
        PRIVATE
            ${BLAS_LIBRARIES}
    )
endforeach()

# stop if cmake version below 2.8
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# project name
project(duplex)

# enable fortran, c, and c++ language
enable_language(C CXX)

# project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
    )

# guard against in-source builds and bad build-type strings
include(safeguards)

# detect operating system
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_definitions(-DSYSTEM_LINUX)
endif()
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_definitions(-DSYSTEM_DARWIN)
endif()
if(${CMAKE_SYSTEM_NAME} STREQUAL "AIX")
    add_definitions(-DSYSTEM_AIX)
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-DSYSTEM_WINDOWS)
endif()


# detect host processor
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # using Clang
    set(CMAKE_CXX_FLAGS         
    "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused -Wc++11-extensions -std=c++14")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3 -Wc++11-extensions -std=c++14")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wc++11-extensions -std=c++14")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif()


# build executable
add_executable(duplex
                src/main.cpp
                src/core        

                src/core/data.cpp
                src/core/dstate.cpp
                src/core/kdtree.c
                src/core/log.cpp
                src/core/search.cpp
                src/core/sensitivity.cpp
                src/core/stat.cpp
                src/core/state.cpp
                src/duplex/optimizer/adadelta.cpp
                src/duplex/optimizer/adagrad.cpp
                src/duplex/optimizer/adam.cpp
                src/duplex/optimizer/gradientDescent.cpp
                src/duplex/optimizer/nadam.cpp
                src/duplex/optimizer/optimizer.cpp

                src/duplex/RRT.cpp
                src/duplex/classification.cpp
                src/duplex/clustering.cpp
                src/duplex/descentOptimizer.cpp
                src/duplex/duplex.cpp
                src/duplex/functionalOptimizer.cpp
                src/duplex/nonconvexOptimizer.cpp
                src/duplex/optimizer
                src/duplex/systemOptimizer.cpp

                src/system/hspice.cpp
                src/system/system.cpp

                src/utility/configuration.cpp
                src/utility/graphics.cpp
                src/utility/plotFactory.cpp
                )


# location of header files
include_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/core
    ${PROJECT_SOURCE_DIR}/include/duplex
    ${PROJECT_SOURCE_DIR}/include/duplex/optimizer
    ${PROJECT_SOURCE_DIR}/include/system
    ${PROJECT_SOURCE_DIR}/include/utility
    )

find_package(Boost 1.51 COMPONENTS program_options REQUIRED)
find_package(config4cpp REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(${Boost_INCLUDE_DIR} ${CONFIG4CPP_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR})
target_link_libraries(duplex ${Boost_LIBRARIES} ${CONFIG4CPP_LIBRARY})

project(rmc)

# required cmake version
cmake_minimum_required(VERSION 2.8.8)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

# global needed variables
set(APPLICATION_NAME ${PROJECT_NAME})
set(TOP_PRJ_DIR ${PROJECT_SOURCE_DIR})

# add definitions
include(DefineCMakeDefaults)
include(DefineCompilerFlags)
include(DefineInstallationPaths)
include(DefineOptions)

# search for libraries
find_package(GMP REQUIRED)

set(RMC_PUBLIC_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    CACHE INTERNAL "rmc public include directories"
)

set(RMC_PRIVATE_INCLUDE_DIRS
    ${GMP_INCLUDE_DIRS}
)

include_directories(
    ${RMC_PUBLIC_INCLUDE_DIRS}
    ${RMC_PRIVATE_INCLUDE_DIRS}
)

add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(src)

cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

project(CsPaint)

include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckTypeSize)
include(FeatureSummary)
include(CompileShaders)

if(UNIX)
    include(GNUInstallDirs)
else()
    set(CMAKE_INSTALL_BINDIR bin)
    set(CMAKE_INSTALL_LIBDIR lib)
    set(CMAKE_INSTALL_INCLUDEDIR include)
endif()

# file locations for building
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(sdl2)
set_package_properties(sdl2 PROPERTIES
   DESCRIPTION "Used only by the demo"
   URL "https://www.libsdl.org"
   TYPE RECOMMENDED
)


find_package(Vulkan)
set_package_properties(Vulkan PROPERTIES
   DESCRIPTION "Required to build CsPaint"
   URL "https://vulkan.lunarg.com"
   TYPE REQUIRED
)

include(src/cspaint.cmake)
include(demo/demo.cmake)

# turn off glm tests
option(GLM_TEST_ENABLE_CXX_17 "" ON)
option(GLM_TEST_ENABLE "" OFF)

add_subdirectory(src/3rdparty/glm)

feature_summary(WHAT PACKAGES_FOUND PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)

cmake_policy(VERSION 3.18.0..3.29.6)

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.19.0")
   # allows spaces in ctest names
   cmake_policy(SET CMP0110 NEW)
endif()

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20.0")
   # enable RTTI on MSVC
   cmake_policy(SET CMP0117 OLD)
endif()

set(SDL2_NO_MWINDOWS on)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

project(CsPaint)

set(BUILD_MAJOR "1")
set(BUILD_MINOR "1")
set(BUILD_MICRO "0")

include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckTypeSize)
include(FeatureSummary)
include(CompileShaders)

# file locations for building
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# file locations for installing
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
   include(GNUInstallDirs)
   set(CMAKE_INSTALL_RPATH "@executable_path")

elseif(CMAKE_SYSTEM_NAME MATCHES "(Linux|OpenBSD|FreeBSD|NetBSD|DragonFly)")
   include(GNUInstallDirs)
   set(CMAKE_INSTALL_RPATH "\$ORIGIN")

elseif(MSVC)
   # use defaults

elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")

   set(CMAKE_INSTALL_BINDIR .)

   set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
   include(cmake/InstallMinGW.cmake)

endif()

find_package(sdl2 NAMES SDL2 sdl2)
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

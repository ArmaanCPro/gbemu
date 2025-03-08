cmake_minimum_required (VERSION 3.28)


# MINGW keyword doesn't always work. However, selecting the preset usually works. VCPKG_CM... is defined by preset
if (MINGW OR VCPKG_CMAKE_SYSTEM STREQUAL MINGW)
    set(VCPKG_TARGET_TRIPLET "x64-mingw-static" CACHE STRING "")
    set(VCPKG_HOST_TRIPLET "x64-mingw-static" CACHE STRING "")
endif()

# enforce static linking in msvc. not sure if this is needed
if(MSVC OR VCPKG_CMAKE_SYSTEM STREQUAL MSVC)
    set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
    set(VCPKG_HOST_TRIPLET "x64-windows-static" CACHE STRING "")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(
            $<$<CONFIG:>:/MT>
            $<$<CONFIG:Debug>:/MTd>
            $<$<CONFIG:Release>:/MT>
    )
endif()

# vcpkg
if (DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
else ()
    message(STATUS "Downloading vcpkg")
    include(FetchContent)
    FetchContent_Declare(vcpkg
            GIT_REPOSITORY https://github.com/microsoft/vcpkg/
    )
    FetchContent_MakeAvailable(vcpkg)
    set(VCPKG_ROOT ${vcpkg_SOURCE_DIR})
endif()
message(STATUS "Using VCPKG_ROOT: ${VCPKG_ROOT}")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")

# for CI, build tests only
option(BUILD_TESTS_ONLY "Build Only Tests" OFF)
if (NOT BUILD_TESTS_ONLY)
    set(VCPKG_MANIFEST_FEATURES "app")
endif()

#--------
# project
# -------
project (gbemu)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif() # compiler automatically adds NDEBUG if not debug mode

# platform specific stuff
if(WIN32)
    set(GB_PLATFORM_DEFINITIONS GB_WINDOWS)
elseif(APPLE)
    set(GB_PLATFORM_DEFINITIONS GB_OSX)
elseif(UNIX AND NOT APPLE)
    set(GB_PLATFORM_DEFINITIONS GB_LINUX)
endif()
add_compile_definitions(${GB_PLATFORM_DEFINITIONS})

add_subdirectory(core)

# testing stuff
enable_testing()
include(CTest)
add_subdirectory(tests)

if (NOT BUILD_TESTS_ONLY)
    set(VCPKG_MANIFEST_FEATURES "app")
    add_subdirectory(app)
endif()

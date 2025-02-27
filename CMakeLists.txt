cmake_minimum_required (VERSION 3.28)

# link vcpkg packages.
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


project (gbemu)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(core)
add_subdirectory(app)

# testing stuff
enable_testing()
include(CTest)
add_subdirectory(tests)

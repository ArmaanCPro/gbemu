cmake_minimum_required (VERSION 3.28)

project (core)

if(MSVC)
    add_compile_options(/MP)				#Use multiple processors when building
    add_compile_options(/W4 /wd4201 /WX)	#Warning level 4, all warnings are errors
else()
    add_compile_options(-W -Wall -Werror) #All Warnings, all warnings are errors
endif()

set  (SOURCES
        "src/cpu.h"
        "src/cpu.cpp"
        "resources/dmg_boot.h"
        "src/memory_map.h"
        "resources/dmg_opcodes.h"
        "src/ppu.h"
        "src/ppu.cpp"
        "src/window.h"
        "src/window.cpp"
)

source_group("src" FILES ${SOURCES})


add_library( core ${SOURCES} )

target_include_directories( core PUBLIC "${PROJECT_SOURCE_DIR}/src" )

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
target_link_libraries(core PUBLIC glfw glad::glad)

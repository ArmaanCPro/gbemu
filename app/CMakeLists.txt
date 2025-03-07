cmake_minimum_required (VERSION 3.28)

project (app)

if(MSVC)
    add_compile_options(/MP)				#Use multiple processors when building
    add_compile_options(/W4 /wd4201 /WX)	#Warning level 4, all warnings are errors
else()
    add_compile_options(-W -Wall -Werror) #All Warnings, all warnings are errors
endif()

set  (SOURCES
        "src/main.cpp"
        "src/window.h"
        "src/window.cpp"
        "src/fb_renderer.h"
        "src/fb_renderer.cpp"
)

add_executable( app ${SOURCES} )
add_dependencies( app core )
target_link_libraries( app core )

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
target_link_libraries(core PUBLIC glfw glad::glad)

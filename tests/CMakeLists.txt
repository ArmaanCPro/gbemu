cmake_minimum_required(VERSION 3.28)

project(tests)

if(MSVC)
    add_compile_options(/MP)				#Use multiple processors when building
    add_compile_options(/W4 /wd4201 /WX)	#Warning level 4, all warnings are errors
else()
    add_compile_options(-W -Wall -Werror)   #All Warnings, all warnings are errors
endif()

set  (SOURCES
        "src/main.cpp"
        "src/tests.cpp"
)

source_group("src" FILES ${SOURCES})


add_executable( tests ${SOURCES} )
add_dependencies( tests core )
target_link_libraries(tests core)

find_package(GTest CONFIG REQUIRED)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

target_link_libraries(
        tests
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)

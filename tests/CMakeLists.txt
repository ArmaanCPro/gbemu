cmake_minimum_required(VERSION 3.28)

project(tests)

if(MSVC)
    add_compile_options(/MP)				#Use multiple processors when building
    add_compile_options(/W4 /wd4201 /WX)	#Warning level 4, all warnings are errors
    add_definitions(-DGTEST_LINKED_AS_SHARED_LIBRARY=0)
    add_definitions(-DGTEST_CREATE_SHARED_LIBRARY=0)
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
target_link_libraries(tests PRIVATE core)

find_package(GTest CONFIG REQUIRED)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)

target_link_libraries(
        tests
        PRIVATE
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)

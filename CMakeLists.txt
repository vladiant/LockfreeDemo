cmake_minimum_required(VERSION 3.5)
project(lockfree_demo)

add_compile_options("-O3")

include_directories( include )

add_executable(
    demo
    demo_main.cpp
)

set_target_properties(
    demo
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_compile_options(
    demo
    PRIVATE
         $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
         $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Linked to atomic for clang
target_link_libraries(demo -latomic)

add_subdirectory(test)

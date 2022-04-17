cmake_minimum_required(VERSION 3.5)
project(lockfree_demo)

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
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -O3 -Werror>
)

target_include_directories(
    demo
    PRIVATE
        include
)

# Linked to atomic for clang
target_link_libraries(
    demo
       $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-latomic>
)

# Tests
include(CTest)

enable_testing()

find_package(Threads REQUIRED)

# exchange_buffer_test
add_executable(
    exchange_buffer_test
    test/main.cpp
    test/exchange_buffer_test.cpp
)

set_target_properties(
    exchange_buffer_test
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(
    exchange_buffer_test
    PRIVATE
        include
        test
)

target_compile_options(
    exchange_buffer_test
    PRIVATE
         $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /Od>
         $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -O0 -Werror>
)

# Linked to atomic for clang
target_link_libraries(
    exchange_buffer_test
    PRIVATE
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-latomic>
)

add_test(NAME exchange_buffer_test COMMAND exchange_buffer_test)

# exchange_buffer_stresstest
add_executable(
    exchange_buffer_stresstest
    test/main.cpp
    test/exchange_buffer_stresstest.cpp
)

set_target_properties(
    exchange_buffer_stresstest
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_compile_options(
    exchange_buffer_stresstest
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /Od>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -O0 -Werror>
)

target_include_directories(
    exchange_buffer_stresstest
    PRIVATE
        include
        test
)

# Linked to atomic for clang
target_link_libraries(
    exchange_buffer_stresstest
    PRIVATE
        Threads::Threads
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-latomic>
)

add_test(NAME exchange_buffer_stresstest COMMAND exchange_buffer_stresstest)

# sync_counter_stresstest
add_executable(
    sync_counter_stresstest
        test/main.cpp
        test/sync_counter_stresstest.cpp
        test/sync_counter.hpp
        test/sync_counter.cpp
)

set_target_properties(
    sync_counter_stresstest
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_compile_options(
    sync_counter_stresstest
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /Od>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -O0 -Werror>
)

target_include_directories(
    sync_counter_stresstest
    PRIVATE
        include
        test
)

# Linked to atomic for clang
target_link_libraries(
    sync_counter_stresstest
    PRIVATE
        Threads::Threads
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-latomic>
)

add_test(NAME sync_counter_stresstest COMMAND sync_counter_stresstest)

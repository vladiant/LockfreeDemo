cmake_minimum_required(VERSION 3.5)
project(lockfree_test)

include(CTest)

add_compile_options("-O0")

enable_testing()

find_package(Threads REQUIRED)

include_directories(.)

add_executable(
    exchange_buffer_test
    main.cpp
    exchange_buffer_test.cpp
)

set_target_properties(
    exchange_buffer_test
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_compile_options(
    exchange_buffer_test
    PRIVATE
         $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
         $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_test(NAME exchange_buffer_test COMMAND exchange_buffer_test)


add_executable(
    exchange_buffer_stresstest
    main.cpp
    exchange_buffer_stresstest.cpp
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
         $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
         $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Linked to atomic for clang
target_link_libraries(exchange_buffer_stresstest Threads::Threads -latomic)

add_test(NAME exchange_buffer_stresstest COMMAND exchange_buffer_stresstest)


add_executable(sync_counter_stresstest
    main.cpp
    sync_counter_stresstest.cpp
    sync_counter.hpp
    sync_counter.cpp
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
         $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
         $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Linked to atomic for clang
target_link_libraries(sync_counter_stresstest Threads::Threads -latomic)

add_test(NAME sync_counter_stresstest COMMAND sync_counter_stresstest)
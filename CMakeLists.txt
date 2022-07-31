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
       $<$<NOT:$<OR:$<CXX_COMPILER_ID:MSVC>,$<PLATFORM_ID:Darwin>>>:-latomic>
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
        $<$<NOT:$<OR:$<CXX_COMPILER_ID:MSVC>,$<PLATFORM_ID:Darwin>>>:-latomic>
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
        $<$<NOT:$<OR:$<CXX_COMPILER_ID:MSVC>,$<PLATFORM_ID:Darwin>>>:-latomic>
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
        $<$<NOT:$<OR:$<CXX_COMPILER_ID:MSVC>,$<PLATFORM_ID:Darwin>>>:-latomic>
)

# TODO: Fix test
# add_test(NAME sync_counter_stresstest COMMAND sync_counter_stresstest)

# Test coverage
find_program(GCOV_PATH gcov)
if (NOT GCOV_PATH)
    message(WARNING "program gcov not found")
endif()

find_program(LCOV_PATH lcov)
if (NOT LCOV_PATH)
    message(WARNING "program lcov not found")
endif()

find_program(GENHTML_PATH genhtml)
if (NOT GENHTML_PATH)
    message(WARNING "program genhtml not found")
endif()

if (LCOV_PATH AND GCOV_PATH)

    target_compile_options(
        exchange_buffer_test
        PRIVATE
            -g -O0 -fprofile-arcs -ftest-coverage
    )

    target_compile_options(
        exchange_buffer_stresstest
        PRIVATE
            -g -O0 -fprofile-arcs -ftest-coverage
    )

    target_compile_options(
        sync_counter_stresstest
        PRIVATE
            -g -O0 -fprofile-arcs -ftest-coverage
    )

    target_link_libraries(
        exchange_buffer_test
        PRIVATE
            --coverage
    )

    target_link_libraries(
        exchange_buffer_stresstest
        PRIVATE
            --coverage
    )

    target_link_libraries(
        sync_counter_stresstest
        PRIVATE
            --coverage
    )

    set(covname cov)

    add_custom_target(cov_data
        # Cleanup lcov
        ${LCOV_PATH} --directory . --zerocounters

        # Run tests
        COMMAND ctest

        # Capturing lcov counters and generating report
        COMMAND ${LCOV_PATH} --directory . --capture --output-file ${covname}.info
        COMMAND ${LCOV_PATH} --remove ${covname}.info
                             '${CMAKE_SOURCE_DIR}/lib/include/*'
                             '${CMAKE_SOURCE_DIR}/test/*'
                             '${CMAKE_SOURCE_DIR}/utest/*'
                             '${CMAKE_SOURCE_DIR}/fakeit/*'
                             '/usr/*'
                             --output-file ${covname}.info.cleaned
    )

    if (GENHTML_PATH)
        add_custom_target(cov
            # Cleanup lcov
            ${LCOV_PATH} --directory . --zerocounters

            # Run tests
            COMMAND ctest

            # Capturing lcov counters and generating report
            COMMAND ${LCOV_PATH} --directory . --capture --output-file ${covname}.info
            COMMAND ${LCOV_PATH} --remove ${covname}.info
                                 '${CMAKE_SOURCE_DIR}/test/*'
                                 '/usr/*'
                                 --output-file ${covname}.info.cleaned
            COMMAND ${GENHTML_PATH} -o ${covname} ${covname}.info.cleaned
            COMMAND ${CMAKE_COMMAND} -E remove ${covname}.info ${covname}.info.cleaned

            COMMENT "Resetting code coverage counters to zero.\nProcessing code coverage counters and generating report."
        )
    else()
        message(WARNING "unable to generate coverage report: missing genhtml")
    endif()

else()
    message(WARNING "unable to add coverage targets: missing coverage tools")
endif()

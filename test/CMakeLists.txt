cmake_minimum_required(VERSION 3.5)
project(lockfree_test)

include(CTest)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options("-O0")

enable_testing()

find_package(Threads REQUIRED)

include_directories(.)

add_executable(exchange_buffer_test
    main.cpp
    exchange_buffer_test.cpp
)

add_test(NAME exchange_buffer_test COMMAND exchange_buffer_test)


add_executable(exchange_buffer_stresstest
    main.cpp
    exchange_buffer_stresstest.cpp
)

# Linked to atomic for clang
target_link_libraries(exchange_buffer_stresstest Threads::Threads -latomic)

add_test(NAME exchange_buffer_stresstest COMMAND exchange_buffer_stresstest)


add_executable(sync_counter_stresstest
    main.cpp
    sync_counter_stresstest.cpp
    ../src/sync_counter.cpp
#this is not nice but will do for now
)

# Linked to atomic for clang
target_link_libraries(sync_counter_stresstest Threads::Threads -latomic)

add_test(NAME sync_counter_stresstest COMMAND sync_counter_stresstest)
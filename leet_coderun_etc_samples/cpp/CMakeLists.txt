cmake_minimum_required(VERSION 3.22)

project(СheckLeetCoderun VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(leet_tasks)
add_subdirectory(me_tasks)

add_executable(check_leet_coderun
    main.cpp
)

target_link_libraries(check_leet_coderun
    PRIVATE
    LeetTasksLib
    MeTasksLib
)

target_include_directories(check_leet_coderun
    PRIVATE
    leet_tasks
    me_tasks
)




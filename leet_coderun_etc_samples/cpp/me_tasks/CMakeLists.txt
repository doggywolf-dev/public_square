add_library(MeTasksLib STATIC
    me_crossstitch_calc.cpp
)

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils
    ${CMAKE_CURRENT_BINARY_DIR}/../utils
)

target_link_libraries(MeTasksLib
    PRIVATE
    StatUtilsLib
)

target_include_directories(MeTasksLib
    PUBLIC
    ../utils
    .
)

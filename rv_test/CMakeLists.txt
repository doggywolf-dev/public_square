cmake_minimum_required(VERSION 3.22)
project(TestTracker VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED)

add_subdirectory(lib)

add_executable(bin_pkrv_test main.cpp)

# Связывание с OpenCV
target_link_libraries(bin_pkrv_test
    ${OpenCV_LIBS}
    PkrvTestLib
)

target_include_directories(bin_pkrv_test
    PRIVATE
    lib
)

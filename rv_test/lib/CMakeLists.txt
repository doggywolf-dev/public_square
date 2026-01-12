add_library(PkrvTestLib
    STATIC
    tracker.cpp
)

find_package(OpenCV REQUIRED)

# Связывание с OpenCV
target_link_libraries(PkrvTestLib
    ${OpenCV_LIBS}
)

target_include_directories(PkrvTestLib PUBLIC .)

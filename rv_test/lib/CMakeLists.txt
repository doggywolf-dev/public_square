cmake_minimum_required(VERSION 3.22)
add_library(PkrvTestLib
    STATIC
    tracker.cpp
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------- OpenCV ---------
find_package(OpenCV REQUIRED)

# --------- ONNX Runtime ---------
set(ONNXRUNTIME_DIR
	"~/libs/onnxruntime-linux-x64-1.23.2"
)

include_directories(
    ${ONNXRUNTIME_DIR}/include
)

# Связывание с OpenCV
target_link_libraries(PkrvTestLib
    ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so
    ${OpenCV_LIBS}
    pthread
)

target_include_directories(PkrvTestLib PUBLIC .
    ${OpenCV_INCLUDE_DIRS}
)

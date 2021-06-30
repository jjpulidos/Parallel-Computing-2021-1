cmake_minimum_required(VERSION 3.17)
project(Parallel-Computing-2021-1)
set(OpenCV_DIR /Users/juanpulido/OpenCV/build_opencv)
set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(Parallel-Computing-2021-1 input.cpp image-effect-par.cpp )

target_link_libraries(Parallel-Computing-2021-1 ${OpenCV_LIBS} )


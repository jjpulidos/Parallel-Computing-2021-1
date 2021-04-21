cmake_minimum_required(VERSION 3.17)
project(opencvTest)
set(OpenCV_DIR /Users/juanpulido/OpenCV/build_opencv)
set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(opencvTest main.cpp )
SET(./path.png)
target_link_libraries(opencvTest ${OpenCV_LIBS} )


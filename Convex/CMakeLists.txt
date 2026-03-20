cmake_minimum_required(VERSION 3.16)
project(Convex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

add_executable(convex
    src/main.cpp
)

target_include_directories(convex PRIVATE
    include
)

target_link_libraries(convex PRIVATE
    ${OpenCV_LIBS}
)

cmake_minimum_required(VERSION 3.14)
project(LineasPuntos CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Build type ────────────────────────────────────────────────────────────────
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(-O3)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g -O0)
endif()

# ── OpenCV ────────────────────────────────────────────────────────────────────
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include: ${OpenCV_INCLUDE_DIRS}")

# ── Sources ───────────────────────────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/filter_utils.cpp
    src/filter_log.cpp
    src/filter_zero_crossing.cpp
    src/filter_sobel.cpp
    src/filter_scharr.cpp
    src/filter_laplacian.cpp
    src/filter_sobel_magnitude.cpp
    src/filter_hough.cpp
    src/filter_canny.cpp
)

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(LineasPuntos ${SOURCES})

target_include_directories(LineasPuntos PRIVATE
    include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(LineasPuntos
    opencv_core
    opencv_imgproc
    opencv_highgui
    opencv_videoio
    opencv_imgcodecs
)

message(STATUS "Build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX std    : C++${CMAKE_CXX_STANDARD}")

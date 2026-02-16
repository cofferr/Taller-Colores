cmake_minimum_required(VERSION 3.10)
project(TallerOpenCV)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

# Detectar automáticamente todos los .cpp
file(GLOB SOURCES "*.cpp")

foreach(src ${SOURCES})
    get_filename_component(name ${src} NAME_WE)
    add_executable(${name} ${src})
    target_link_libraries(${name} ${OpenCV_LIBS})
endforeach()

message(STATUS "OpenCV version: ${OpenCV_VERSION}")
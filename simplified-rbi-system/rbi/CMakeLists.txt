cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
project(rbi VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PostgreSQL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(OpenCLHeaders REQUIRED)
find_package(OpenCLHeadersCpp REQUIRED)

add_subdirectory(src)

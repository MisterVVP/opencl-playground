cmake_minimum_required(VERSION 3.28.0 FATAL_ERROR)
project(sensors VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PostgreSQL REQUIRED)

add_subdirectory(src)
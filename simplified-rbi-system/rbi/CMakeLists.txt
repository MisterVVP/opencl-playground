cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
include(FetchContent)

project(rbi VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# hiredis
FetchContent_Declare(
  hiredis 
  GIT_REPOSITORY https://github.com/redis/hiredis.git 
  GIT_TAG v1.2.0 
  GIT_SHALLOW TRUE 
  GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(hiredis)

# msgpack
FetchContent_Declare(
  msgpack 
  GIT_REPOSITORY https://github.com/msgpack/msgpack-c.git
  GIT_TAG cpp-7.0.0 
  GIT_SHALLOW TRUE 
  GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(msgpack)

find_package(PostgreSQL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(OpenCLHeaders REQUIRED)
find_package(OpenCLHeadersCpp REQUIRED)

add_subdirectory(src)

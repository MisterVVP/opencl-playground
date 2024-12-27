cmake_minimum_required(VERSION 3.28.0 FATAL_ERROR)
project(metrics-api VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

FetchContent_Declare(
  uWebSockets
  GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
  GIT_TAG v20.71.0
)
FetchContent_MakeAvailable(uWebSockets)
find_package(ZLIB REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(nlohmann_json 3.11.3 REQUIRED)

add_subdirectory(src)
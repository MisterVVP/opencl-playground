add_executable(metrics-api main.cpp dao.cpp dao.h)

target_include_directories(metrics-api PRIVATE "${uwebsockets_SOURCE_DIR}/src" "${uwebsockets_SOURCE_DIR}/uSockets/src")

target_link_libraries(metrics-api PRIVATE "${uwebsockets_SOURCE_DIR}/uSockets/uSockets.a" ZLIB::ZLIB PostgreSQL::PostgreSQL nlohmann_json::nlohmann_json)

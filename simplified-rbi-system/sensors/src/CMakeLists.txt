add_executable(sensors main.cpp dao.cpp dao.h)

target_link_libraries(sensors PRIVATE PostgreSQL::PostgreSQL)

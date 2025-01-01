add_executable(${PROJECT_NAME} "main.cpp" "dao.cpp" "dao.h" "cache.h")

target_include_directories(${PROJECT_NAME} PRIVATE "${hiredis_SOURCE_DIR}" "${hiredis_SOURCE_DIR}")

target_link_libraries(${PROJECT_NAME} PRIVATE PostgreSQL::PostgreSQL OpenCL::Headers OpenCL::OpenCL OpenCL::HeadersCpp "${hiredis_SOURCE_DIR}/libhiredis.a" msgpack-cxx)

target_compile_definitions(${PROJECT_NAME} PRIVATE CL_TARGET_OPENCL_VERSION=300)
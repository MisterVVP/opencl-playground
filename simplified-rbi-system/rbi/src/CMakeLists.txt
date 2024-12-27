add_executable(${PROJECT_NAME} main.cpp dao.cpp dao.h)

target_link_libraries(${PROJECT_NAME} PRIVATE PostgreSQL::PostgreSQL OpenCL::Headers OpenCL::OpenCL OpenCL::HeadersCpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE CL_TARGET_OPENCL_VERSION=300)
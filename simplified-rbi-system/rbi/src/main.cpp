#define CL_HPP_TARGET_OPENCL_VERSION 300
#define CL_HPP_MINIMUM_OPENCL_VERSION 300
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include "dao.h"
#include <CL/cl.h>
#include <CL/opencl.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <stdexcept>
#include <unistd.h>

using namespace std;

const char *KERNEL_SOURCE = R"(
__kernel void compute_risk(__global const float *temperature, 
                           __global const float *pressure, 
                           __global float *risk)
{
    int gid = get_global_id(0);
    risk[gid] = fma(temperature[gid], 0.5f, pressure[gid] * 0.3f);
}
)";

std::string get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));
    return std::string(buffer);
}

void compute_risk_indices(const std::vector<float>& temperature, 
                          const std::vector<float>& pressure, 
                          std::vector<float>& risk, 
                          int device_id) {
    // Ensure the risk vector is the correct size
    risk.resize(temperature.size());

    try {
        // Select OpenCL platform and device
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.empty()) {
            throw std::runtime_error("No OpenCL platforms found.");
        }

        cl::Platform platform = platforms[0];
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        if (devices.empty()) {
            throw std::runtime_error("No OpenCL devices found.");
        }

        cl::Device device = devices[0];
        cl::Context context(device);
        cl::CommandQueue queue(context, device);

        // Create and build OpenCL program
        const char* KERNEL_SOURCE = R"(
            __kernel void compute_risk(__global const float* temperature, 
                                       __global const float* pressure, 
                                       __global float* risk) {
                int i = get_global_id(0);
                risk[i] = temperature[i] * pressure[i] / 100000.0f;
            }
        )";
        cl::Program::Sources sources;
        sources.push_back({KERNEL_SOURCE, strlen(KERNEL_SOURCE)});
        cl::Program program(context, sources);

        try {
            program.build({device});
        } catch (const cl::Error& err) {
            std::cerr << "Build Error: " 
                      << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) 
                      << std::endl;
            throw;
        }

        // Create buffers for temperature, pressure, and risk
        cl::Buffer tempBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                              temperature.size() * sizeof(float), 
                              const_cast<float*>(temperature.data()));
        cl::Buffer pressBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                               pressure.size() * sizeof(float), 
                               const_cast<float*>(pressure.data()));
        cl::Buffer riskBuffer(context, CL_MEM_WRITE_ONLY, 
                              risk.size() * sizeof(float));

        // Create kernel and set arguments
        cl::Kernel kernel(program, "compute_risk");
        kernel.setArg(0, tempBuffer);
        kernel.setArg(1, pressBuffer);
        kernel.setArg(2, riskBuffer);

        // Execute the kernel
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(temperature.size()));
        queue.enqueueReadBuffer(riskBuffer, CL_TRUE, 0, risk.size() * sizeof(float), risk.data());

        // Log computation for the specific device
        std::cout << "Computed risk indices for device " << device_id << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during OpenCL risk computation: " << e.what() << std::endl;
    }
}

void generate_rbi_reports(int device_id) {
    try {
        const auto* psql_conn_str = std::getenv("POSTGRES_CONNECTION_STRING");

        // Initialize DAO
        Dao::RBIReportsDao rbiReportsDao(psql_conn_str);
        std::vector<float> temperature = rbiReportsDao.fetchMetricData(1, device_id); // Metric ID 1: Temperature
        std::vector<float> pressure = rbiReportsDao.fetchMetricData(2, device_id);    // Metric ID 2: Pressure

        if (temperature.size() != pressure.size()) {
            throw std::runtime_error("Mismatch in temperature and pressure data sizes.");
        }

        // Compute risk indices
        std::vector<float> risk(temperature.size());
        compute_risk_indices(temperature, pressure, risk, device_id);

        // Insert computed risks into the database
        for (size_t i = 0; i < risk.size(); ++i) {
            rbiReportsDao.insertRBIReport(device_id, risk[i], get_current_timestamp());
        }

        std::cout << "RBI reports generated successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error generating RBI reports: " << e.what() << std::endl;
    }
}


int main() {
    const auto* use_dockerized_postgres = std::getenv("USE_DOCKERIZED_POSTGRES");
    if (use_dockerized_postgres) {
        fprintf(stdout, "Sleeping for 3 seconds...\n");
        usleep(3000000);
    }
    // TODO replace hardcoded device id
    generate_rbi_reports(1);
    return 0;
}

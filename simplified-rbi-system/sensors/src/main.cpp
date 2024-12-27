#include "dao.h"
#include <chrono>
#include <random>
#include <ctime>
#include <vector>
#include <unordered_map>
#include <unistd.h>

using namespace std;
using namespace Dao;

string get_current_timestamp() {
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now_time));
    return string(buffer);
}

float generate_random_value(float min = 10.0, float max = 100.0) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(min, max);
    return static_cast<float>(dis(gen));
}

int main() {
    const auto* use_dockerized_postgres = std::getenv("USE_DOCKERIZED_POSTGRES");
    if (use_dockerized_postgres) {
        fprintf(stdout, "Sleeping for 3 seconds...\n");
        usleep(3000000);
    }
    const auto* psql_conn_str = std::getenv("POSTGRES_CONNECTION_STRING");

    Dao::SensorsDao sensorsDao(psql_conn_str);

    try {
        // Sensor-to-Device Mapping
        unordered_map<int, int> sensor_to_device = {
            {1, 1}, // temp_sensor_1 -> Device_A
            {2, 1}, // press_sensor_1 -> Device_A
            {3, 2}, // temp_sensor_2 -> Device_B
            {4, 2}  // press_sensor_2 -> Device_B
        };

        // Sensor IDs and Metric IDs
        vector<int> sensor_ids = {1, 2, 3, 4};
        unordered_map<int, pair<float, float>> metric_ranges = {
            {1, {15.0, 30.0}}, // Temperature
            {2, {100000.0, 102000.0}} // Pressure
        };

        while (true) {
            for (int sensor_id : sensor_ids) {
                // Determine the associated device and metric
                int device_id = sensor_to_device[sensor_id];
                int metric_id = sensor_id % 2 == 1 ? 1 : 2; // Alternate between temperature and pressure
                auto [min_value, max_value] = metric_ranges[metric_id];

                // Generate data
                float value = generate_random_value(min_value, max_value);
                string timestamp = get_current_timestamp();

                // Insert sensor reading
                sensorsDao.insertSensorReading(sensor_id, device_id, metric_id, value, timestamp);
            }
            cout << "Inserted sensor readings at " << get_current_timestamp() << endl;
            usleep(3000000); // 3-second delay
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
    return 0;
}

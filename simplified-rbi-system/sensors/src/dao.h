#pragma once
#include <string>
#include <libpq-fe.h>
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace Dao
{
    class ISensorsDao {
    public:
        virtual int insertSensorReading(int sensor_id, int device_id, int metric_id, float value, const std::string &timestamp) = 0;
        virtual ~ISensorsDao() = default;  // Virtual destructor for proper cleanup
    };

    class SensorsDao : public ISensorsDao {
    private:
        PGconn *conn;

    public:
        SensorsDao(char const* psql_conn_str);
        ~SensorsDao() override;

        int insertSensorReading(int sensor_id, int device_id, int metric_id, float value, const std::string &timestamp) override;
    };
}

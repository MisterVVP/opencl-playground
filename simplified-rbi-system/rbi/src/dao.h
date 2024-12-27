#pragma once
#include <string>
#include <libpq-fe.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>
#include <CL/cl.h>

namespace Dao
{
    class IRBIReportsDao {
    public:
        virtual void insertRBIReport(int device_id, float risk_index, const std::string &generated_at) = 0;
        virtual std::vector<float> fetchMetricData(int metric_id, int device_id) = 0;
        virtual ~IRBIReportsDao() = default;  // Virtual destructor for proper cleanup
    };

    class RBIReportsDao : public IRBIReportsDao {
    private:
        PGconn *conn;

    public:
        RBIReportsDao(char const* psql_conn_str);
        ~RBIReportsDao() override;

        void insertRBIReport(int device_id, float risk_index, const std::string &generated_at) override;
        std::vector<float> fetchMetricData(int metric_id, int device_id) override;
    };
}

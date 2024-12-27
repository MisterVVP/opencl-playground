#pragma once
#include <vector>
#include <libpq-fe.h>
#include <stdexcept>
#include <iostream>
#include <string>

namespace Dao
{
    struct Metrics {
        int id;
        std::string name;
        std::string unit;
        std::string description;
        std::string created_at;
    };
     
    class IMetricsDao {
        public:    
            virtual std::vector<Metrics> fetchMetrics() = 0;
            virtual ~IMetricsDao() = default;  // Virtual destructor for proper cleanup
    };

    class MetricsDao : public IMetricsDao {
        private:
            PGconn *conn;
        public:
            MetricsDao(char const* psql_conn_str);
            ~MetricsDao() override;
            std::vector<Metrics> fetchMetrics() override;
    };
}

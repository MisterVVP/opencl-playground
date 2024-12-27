#include "dao.h"

using namespace Dao;


Dao::RBIReportsDao::RBIReportsDao(char const* psql_conn_str) {
    if (!psql_conn_str) {
        throw std::invalid_argument("POSTGRES_CONNECTION_STRING is not defined");
    }
    conn = PQconnectdb(psql_conn_str);
    if (PQstatus(conn) != CONNECTION_OK) {
        throw std::runtime_error(PQerrorMessage(conn));
    } else {
        std::cout << "Connected to database\n";
    }
}

Dao::RBIReportsDao::~RBIReportsDao()
{
    PQfinish(conn);
}

void Dao::RBIReportsDao::insertRBIReport(int device_id, float risk_index, const std::string &generated_at) {
    std::ostringstream query;
    query << "INSERT INTO rbi_reports (device_id, risk_index, generated_at) VALUES ("
          << device_id << ", " << risk_index << ", '" << generated_at << "');";

    PGresult *res = PQexec(conn, query.str().c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Insert failed: " << PQerrorMessage(conn) << std::endl;
    }
    PQclear(res);
}

std::vector<float> Dao::RBIReportsDao::fetchMetricData(int metric_id, int device_id) {
    std::ostringstream query;
    query << "SELECT value FROM sensor_readings WHERE metric_id = " << metric_id << " AND device_id = " << device_id << ";";

    PGresult *res = PQexec(conn, query.str().c_str());
    std::vector<float> data;

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; ++i) {
            data.push_back(std::stof(PQgetvalue(res, i, 0)));
        }
    } else {
        std::cerr << "Query failed: " << PQerrorMessage(conn) << std::endl;
    }

    PQclear(res);
    return data;
}

#include "dao.h"

using namespace Dao;

Dao::SensorsDao::SensorsDao(char const *psql_conn_str)
{
    if (!psql_conn_str) {
        throw std::invalid_argument("POSTGRES_CONNECTION_STRING is not defined");
    }
    /* Make a connection to the database */
    conn = PQconnectdb(psql_conn_str);
    /* Check to see that the backend connection was successfully made */
    if (PQstatus(conn) != CONNECTION_OK) {
        throw std::runtime_error(PQerrorMessage(conn));
    } else {
        fprintf(stdout, "Connected to database\n");
    }
}

Dao::SensorsDao::~SensorsDao()
{
    PQfinish(conn);
}

int Dao::SensorsDao::insertSensorReading(int sensor_id, int device_id, int metric_id, float value, const std::string &timestamp)
{
    std::ostringstream query;
    query << "INSERT INTO sensor_readings (sensor_id, device_id, metric_id, value, timestamp) VALUES ("
          << sensor_id << ", " << device_id << ", " << metric_id << ", " << value << ", '" << timestamp << "') RETURNING id;";

    PGresult *res = PQexec(conn, query.str().c_str());

    int inserted_id = -1;
    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        inserted_id = std::stoi(PQgetvalue(res, 0, 0));
    } else {
        std::cerr << "Insert failed: " << PQerrorMessage(conn) << std::endl;
    }

    PQclear(res);
    return inserted_id;
}

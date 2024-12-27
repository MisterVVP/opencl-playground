#include "dao.h"

using namespace Dao;

Dao::MetricsDao::MetricsDao(char const *psql_conn_str)
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

Dao::MetricsDao::~MetricsDao()
{
    PQfinish(conn);
}

std::vector<Metrics> Dao::MetricsDao::fetchMetrics()
{
    std::vector<Metrics> metricsList;

    // SQL query to select all records from the metrics table
    const char* query = "SELECT id, name, unit, description, created_at FROM metrics";
    
    // Execute the query
    PGresult* result = PQexec(conn, query);

    // Check for errors in query execution
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        std::cerr << "SELECT query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(result);
        return metricsList;
    }

    // Iterate through the rows and populate the Metrics vector
    int rows = PQntuples(result);
    for (int i = 0; i < rows; i++) {
        Metrics metric;
        metric.id = std::stoi(PQgetvalue(result, i, 0));  // Column 0: id
        metric.name = PQgetvalue(result, i, 1);          // Column 1: name
        metric.unit = PQgetvalue(result, i, 2);          // Column 2: unit
        metric.description = PQgetvalue(result, i, 3);   // Column 3: description
        metric.created_at = PQgetvalue(result, i, 4);    // Column 4: created_at

        metricsList.push_back(metric);
    }

    // Free the result object
    PQclear(result);

    return metricsList;
}

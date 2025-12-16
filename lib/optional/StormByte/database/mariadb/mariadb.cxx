#include <StormByte/database/mariadb/mariadb.hxx>
#include <StormByte/database/mariadb/result_fetch.hxx>
#include <StormByte/database/mariadb/prepared_stmt.hxx>
#include <mysql.h>
#include <iostream>

using namespace StormByte::Database::MariaDB;

MariaDB::~MariaDB() noexcept {
    Disconnect();
}

MariaDB::MariaDB(const std::string& host, const std::string& user, const std::string& password, const std::string& db_name, int port, std::shared_ptr<Logger::Log> logger):
    Database(logger), m_host(host), m_user(user), m_password(password), m_dbname(db_name), m_port(port), m_conn(nullptr) {}

MariaDB::MariaDB(std::string&& host, std::string&& user, std::string&& password, std::string&& db_name, int port, std::shared_ptr<Logger::Log> logger):
    Database(logger), m_host(std::move(host)), m_user(std::move(user)), m_password(std::move(password)), m_dbname(std::move(db_name)), m_port(port), m_conn(nullptr) {}

bool MariaDB::DoConnect() noexcept {
    if (m_connected) return false;

    MYSQL* conn = mysql_init(nullptr);
    if (!conn) return false;
#if defined(MYSQL_OPT_SSL_MODE) && defined(SSL_MODE_PREFERRED)
    /* Prefer SSL but do not require it. Some CI images have a MariaDB
     * server without TLS configured; asking the client to prefer TLS
     * allows TLS when available but falls back to plain TCP when not. */
    int ssl_mode = SSL_MODE_PREFERRED;
    mysql_options(conn, MYSQL_OPT_SSL_MODE, &ssl_mode);
#endif

    unsigned int port = static_cast<unsigned int>(m_port);
    if (!mysql_real_connect(conn, m_host.empty() ? nullptr : m_host.c_str(), m_user.empty() ? nullptr : m_user.c_str(), m_password.empty() ? nullptr : m_password.c_str(), m_dbname.empty() ? nullptr : m_dbname.c_str(), port, nullptr, 0)) {
        m_logger << Logger::Level::Error << "MariaDB connection error: " << (mysql_error(conn) ? mysql_error(conn) : "Unknown error") << std::endl;
        mysql_close(conn);
        m_conn = nullptr;
        return false;
    }
    m_conn = conn;
    return true;
}

void MariaDB::DoPreDisconnect() noexcept {
    m_prepared_stmts.clear();
}

void MariaDB::DoDisconnect() noexcept {
    if (m_conn) {
        mysql_close(m_conn);
        m_conn = nullptr;
    }
}

StormByte::Database::ExpectedRows MariaDB::Query(const std::string& query) noexcept {
    if (!m_connected || !m_conn) return Unexpected<ExecuteError>("Database not connected");

    if (mysql_real_query(m_conn, query.c_str(), static_cast<unsigned long>(query.size())) != 0) {
        return Unexpected<ExecuteError>(mysql_error(m_conn) ? mysql_error(m_conn) : "Unknown MySQL error");
    }

    MYSQL_RES* res = mysql_store_result(m_conn);
    if (!res) {
        if (mysql_field_count(m_conn) == 0) return Rows();
        return Unexpected<ExecuteError>(mysql_error(m_conn) ? mysql_error(m_conn) : "Unknown MySQL error");
    }

    StormByte::Database::ExpectedRows rows = StormByte::Database::MariaDB::StepResults(res);
    mysql_free_result(res);
    return rows;
}

bool MariaDB::SilentQuery(const std::string& query) noexcept {
    if (!m_connected || !m_conn) return false;
    if (mysql_real_query(m_conn, query.c_str(), static_cast<unsigned long>(query.size())) != 0) {
        m_logger << Logger::Level::Error << "MariaDB SilentQuery error: " << (mysql_error(m_conn) ? mysql_error(m_conn) : "Unknown MySQL error") << std::endl;
        return false;
    }
    return true;
}

std::unique_ptr<StormByte::Database::PreparedSTMT> MariaDB::CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept {
    if (!m_conn) return nullptr;
    return std::unique_ptr<PreparedSTMT>(new PreparedSTMT(std::move(name), std::move(query), m_conn));
}

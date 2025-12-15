#include <StormByte/database/postgres/result_fetch.hxx>
#include <StormByte/database/postgres/postgres.hxx>

#include <libpq-fe.h>

using namespace StormByte::Database::Postgres;

Postgres::Postgres(const std::string& host, const std::string& user, const std::string& password, const std::string& db_name, std::shared_ptr<Logger::Log> logger):
	Database(logger), m_host(host), m_user(user), m_password(password), m_dbname(db_name), m_conn(nullptr) {}

Postgres::Postgres(std::string&& host, std::string&& user, std::string&& password, std::string&& db_name, std::shared_ptr<Logger::Log> logger):
	Database(logger), m_host(std::move(host)), m_user(std::move(user)), m_password(std::move(password)), m_dbname(std::move(db_name)), m_conn(nullptr) {}

Postgres::~Postgres() noexcept {
	Disconnect();
}

bool Postgres::DoConnect() noexcept {
	if (m_connected)
		return false;

	// build conninfo from host/user/password
	std::string conninfo = "";
	if (!m_host.empty()) conninfo += "host='" + m_host + "' ";
	if (!m_user.empty()) conninfo += "user='" + m_user + "' ";
	if (!m_password.empty()) conninfo += "password='" + m_password + "' ";
	if (!m_dbname.empty()) conninfo += "dbname='" + m_dbname + "' ";

	PGconn* conn = PQconnectdb(conninfo.c_str());
	if (!conn)
		return false;

	if (PQstatus(conn) != CONNECTION_OK) {
		m_logger << Logger::Level::Error << "Postgres connection error: " << (PQerrorMessage(conn) ? PQerrorMessage(conn) : "Unknown error") << std::endl;
		PQfinish(conn);
		m_conn = nullptr;
		return false;
	}
	m_conn = conn;

	return true;
}

void Postgres::DoPreDisconnect() noexcept {
	m_prepared_stmts.clear();
}

void Postgres::DoDisconnect() noexcept {
	if (m_conn) {
		PQfinish(m_conn);
		m_conn = nullptr;
	}
}

StormByte::Database::ExpectedRows Postgres::Query(const std::string& query) noexcept {
	if (!m_connected || !m_conn)
		return Unexpected<ExecuteError>("Database not connected");

	PGresult* res = PQexec(static_cast<PGconn*>(m_conn), query.c_str());
	if (!res) {
		return Unexpected<ExecuteError>("Null PGresult");
	}

	ExecStatusType st = PQresultStatus(res);
	if (st != PGRES_TUPLES_OK && st != PGRES_COMMAND_OK) {
		std::string err = PQerrorMessage(m_conn) ? PQerrorMessage(m_conn) : "Unknown Postgres error";
		PQclear(res);
		return Unexpected<ExecuteError>(err);
	}

	ExpectedRows rows = StepResults(res);
	PQclear(res);
	return rows;
}

bool Postgres::SilentQuery(const std::string& query) noexcept {
	if (!m_connected || !m_conn)
		return false;

	PGresult* res = PQexec(static_cast<PGconn*>(m_conn), query.c_str());
	if (!res)
		return false;

	ExecStatusType st = PQresultStatus(res);
	if (st != PGRES_COMMAND_OK && st != PGRES_TUPLES_OK) {
		m_logger << Logger::Level::Error << "Postgres SilentQuery error: " << (PQerrorMessage(m_conn) ? PQerrorMessage(m_conn) : "Unknown error") << std::endl;
		PQclear(res);
		return false;
	}
	PQclear(res);
	return true;
}

std::unique_ptr<StormByte::Database::PreparedSTMT> Postgres::CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept {
	if (!m_conn)
		return nullptr;

	PGconn* conn = static_cast<PGconn*>(m_conn);
	// strip trailing semicolons/whitespace to avoid PQprepare errors
	std::string qcopy = query;
	while (!qcopy.empty() && (qcopy.back() == ';' || isspace(static_cast<unsigned char>(qcopy.back())))) qcopy.pop_back();
	PGresult* res = PQprepare(conn, name.c_str(), qcopy.c_str(), 0, nullptr);
	if (!res) {
		m_logger << Logger::Level::Error << "PQprepare returned null for statement '" << name << "'" << std::endl;
		return nullptr;
	}

	ExecStatusType st = PQresultStatus(res);
	if (st != PGRES_COMMAND_OK && st != PGRES_TUPLES_OK) {
		m_logger << Logger::Level::Error << "PQprepare error for statement '" << name << "': " << (PQresultErrorMessage(res) ? PQresultErrorMessage(res) : "Unknown") << std::endl;
		PQclear(res);
		return nullptr;
	}
	PQclear(res);

	std::unique_ptr<PreparedSTMT> stmt = std::make_unique<PreparedSTMT>(PreparedSTMT(std::move(name), std::move(query)));
	stmt->m_conn = m_conn;
	return stmt;
}

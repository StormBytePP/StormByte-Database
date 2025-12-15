#include <StormByte/database/sqlite/sqlite3.hxx>

#include <sqlite3.h>

using namespace StormByte::Database::SQLite;

SQLite3::SQLite3(std::shared_ptr<Logger::Log> logger) noexcept:SQLite3(":memory:", logger) {}

SQLite3::SQLite3(const std::filesystem::path& dbfile, std::shared_ptr<Logger::Log> logger):
Database(logger), m_database_file(dbfile), m_database(nullptr) {}

SQLite3::SQLite3(std::filesystem::path&& dbfile, std::shared_ptr<Logger::Log>&& logger):
Database(std::move(logger)), m_database_file(std::move(dbfile)), m_database(nullptr) {}

SQLite3::~SQLite3() noexcept {
	Disconnect();
}

bool SQLite3::DoConnect() noexcept {
	if (m_connected)
		return false;

	if (sqlite3_open(m_database_file.string().c_str(), &m_database) != SQLITE_OK) {
		sqlite3_close(m_database);
		m_database = nullptr;
		return false;
    }
	return true;
}

void SQLite3::DoPreDisconnect() noexcept {
	if (m_database)
		m_prepared_stmts.clear();
}

void SQLite3::DoDisconnect() noexcept {
	if (m_database) {
		sqlite3_close(m_database);
		m_database = nullptr;
	}
}

void SQLite3::DoPostDisconnect() noexcept {
	sqlite3_shutdown();
	m_database = nullptr;
}

StormByte::Database::ExpectedRows SQLite3::Query(const std::string& query) noexcept {
	if (!m_connected)
		return Unexpected<ExecuteError>("Database not connected");

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_database, query.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		const std::string errorStr = sqlite3_errmsg(m_database);
		if (stmt) sqlite3_finalize(stmt);
		return Unexpected<ExecuteError>(errorStr);
	}
	Rows rows;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		Row row;
		for (auto i = 0; i < sqlite3_column_count(stmt); i++) {
			const char* colName = sqlite3_column_name(stmt, i);
			switch (sqlite3_column_type(stmt, i)) {
				case SQLITE_INTEGER:
				{
					sqlite3_int64 v = sqlite3_column_int64(stmt, i);
					if (v > std::numeric_limits<int>::max() || v < std::numeric_limits<int>::min())
						row.Add(colName, static_cast<long int>(v));
					else
						row.Add(colName, static_cast<int>(v));
					break;
				}
					break;
				case SQLITE_FLOAT:
					row.Add(colName, sqlite3_column_double(stmt, i));
					break;
				case SQLITE_TEXT:
					row.Add(colName, std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i))));
					break;
				case SQLITE_BLOB: {
					const std::byte* blobData = reinterpret_cast<const std::byte*>(sqlite3_column_blob(stmt, i));
					int blobSize = sqlite3_column_bytes(stmt, i);
					std::vector<std::byte> blobVec(blobData, blobData + blobSize);
					row.Add(colName, std::move(blobVec));
					break;
				}
				case SQLITE_NULL:
				default:
					row.Add(colName, nullptr);
					break;
			}
		}
		rows.Add(std::move(row));
	}
	sqlite3_finalize(stmt);
	return rows;
}

bool SQLite3::SilentQuery(const std::string& query) noexcept {
	if (!m_connected)
		return false;

	char* errMsg = nullptr;
	int rc = sqlite3_exec(m_database, query.c_str(), nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK) {
		if (errMsg) {
			const std::string errStr = errMsg ? std::string(errMsg) : "Unknown error";
			m_logger << Logger::Level::Error << "SQLite3 SilentQuery error: " << errStr << std::endl;
			sqlite3_free(errMsg);
		}
		return false;
	}
	return true;
}

void SQLite3::EnableForeignKeys() {
	SilentQuery("PRAGMA foreign_keys = ON;");
}

std::unique_ptr<StormByte::Database::PreparedSTMT> SQLite3::CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept {
	std::unique_ptr<PreparedSTMT> stmt = std::make_unique<PreparedSTMT>(PreparedSTMT(std::move(name), std::move(query)));
	sqlite3_prepare_v2( m_database, stmt->Query().c_str(), static_cast<int>(stmt->Query().length()), &(stmt->m_stmt), nullptr);
	if (!stmt->m_stmt)
		return nullptr;

	return stmt;
}


#include <StormByte/database/sqlite/sqlite3.hxx>

#include <sqlite3.h>

#ifdef MSVC
/** WINDOWS NEEDS EXPLICIT INSTANTIATION TO AVOID LINKER ERRORS **/
namespace StormByte::Database {
	template class Database<SQLite::Query, SQLite::PreparedSTMT>;
	template class Row<SQLite::Value>;
}
#endif

using namespace StormByte::Database::SQLite;

SQLite3::SQLite3():SQLite3(":memory:") {}

SQLite3::SQLite3(const std::filesystem::path& dbfile):m_database_file(dbfile) {
	Connect();
}

SQLite3::SQLite3(std::filesystem::path&& dbfile):m_database_file(std::move(dbfile)) {
	Connect();
}

SQLite3::~SQLite3() noexcept { InternalDisconnect(); }

void SQLite3::Connect() {
	// This is undefined behavior if called more than once for the same object
	// Windows needs this string intermediate conversion
	if (sqlite3_open(m_database_file.string().c_str(), &m_database) != SQLITE_OK) {
		std::string message = "Cannot open database " + m_database_file.string() + ": " + LastError(); // SQLite3 handles internally freeing message's memory
		Disconnect(); // Need to close database here as exception throwing might skip destructor
        throw ConnectionError(std::move(message));
    }
	EnableForeignKeys();
}

void SQLite3::Disconnect() {
	InternalDisconnect();
}

void SQLite3::EnableForeignKeys() {
	SilentQuery("PRAGMA foreign_keys = OFF");
}

const std::string SQLite3::LastError() const {
	return sqlite3_errmsg(m_database);
}

void SQLite3::InternalDisconnect() {
	if (m_database) {
		m_prepared_stmts.clear();
		sqlite3_close(m_database);
		m_database = nullptr;
	}
}

std::unique_ptr<PreparedSTMT> SQLite3::InternalPrepare(const std::string& name, const std::string& query) {
	std::unique_ptr<PreparedSTMT> stmt = std::make_unique<PreparedSTMT>(PreparedSTMT(name, query));
	sqlite3_prepare_v2( m_database, stmt->GetQuery().c_str(), static_cast<int>(stmt->GetQuery().length()), &(stmt->m_stmt), nullptr);
	if (!stmt->m_stmt) {
		throw QueryError("Prepared sentence " + name + " can not be loaded: " + LastError());
	}
	else
		return stmt;
}

std::unique_ptr<Query> SQLite3::InternalQuery(const std::string& query) {
	auto q = std::make_unique<Query>(SQLite::Query(query));
	q->m_stmt = InternalPrepare("Query", query);
	return q;
}

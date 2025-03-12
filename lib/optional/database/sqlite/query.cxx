#include <database/sqlite/query.hxx>

using namespace StormByte::Database::SQLite;

Query::Query(const std::string& query):Database::Query<Row>(query) {}

const Row& Query::Step() noexcept {
	return m_stmt->Step();
}
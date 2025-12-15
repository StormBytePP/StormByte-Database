#include <StormByte/database/sqlite/prepared_stmt.hxx>
#include <StormByte/database/sqlite/result_fetch.hxx>

using namespace StormByte::Database::SQLite;

PreparedSTMT::PreparedSTMT(const std::string& name, const std::string& query):Database::PreparedSTMT(name, query), m_stmt(nullptr) {}

PreparedSTMT::PreparedSTMT(std::string&& name, std::string&& query) noexcept:Database::PreparedSTMT(std::move(name), std::move(query)), m_stmt(nullptr) {}

PreparedSTMT::~PreparedSTMT() noexcept {
	if (m_stmt) {
		sqlite3_finalize(m_stmt);
		m_stmt = nullptr;
	}
}

void PreparedSTMT::Bind(const int& column, const std::nullptr_t&) noexcept {
	sqlite3_bind_null(m_stmt, column + 1);
}

void PreparedSTMT::Bind(const int& column, const int& val) noexcept {
	sqlite3_bind_int(m_stmt, column + 1, val);
}

void PreparedSTMT::Bind(const int& column, const unsigned int& val) noexcept {
	sqlite3_bind_int(m_stmt, column + 1, val);
}

void PreparedSTMT::Bind(const int& column, const int64_t& val) noexcept {
	sqlite3_bind_int64(m_stmt, column + 1, val);
}

void PreparedSTMT::Bind(const int& column, const uint64_t& val) noexcept {
	sqlite3_bind_int64(m_stmt, column + 1, val);
}

void PreparedSTMT::Bind(const int& column, const double& val) noexcept {
	sqlite3_bind_double(m_stmt, column + 1, val);
}

void PreparedSTMT::Bind(const int& column, bool val) noexcept {
	sqlite3_bind_int(m_stmt, column + 1, val);
}

void PreparedSTMT::Bind(const int& column, const std::string& val) noexcept {
	sqlite3_bind_text(m_stmt, column + 1, val.c_str(), -1, SQLITE_STATIC);
}

void PreparedSTMT::Bind(const int& column, const std::vector<std::byte>& val) noexcept {
	if (val.empty()) {
		sqlite3_bind_blob(m_stmt, column + 1, nullptr, 0, SQLITE_TRANSIENT);
	} else {
		sqlite3_bind_blob(m_stmt, column + 1, reinterpret_cast<const void*>(val.data()), static_cast<int>(val.size()), SQLITE_TRANSIENT);
	}
}

void PreparedSTMT::Reset() noexcept {
	sqlite3_clear_bindings(m_stmt);
	sqlite3_reset(m_stmt);
}

StormByte::Database::ExpectedRows PreparedSTMT::DoExecute() {
	return StepResults(m_stmt);
}

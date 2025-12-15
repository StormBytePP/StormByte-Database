#include <StormByte/database/sqlite/prepared_stmt.hxx>

#include <limits>
#include <sqlite3.h>

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

void PreparedSTMT::Reset() noexcept {
	sqlite3_clear_bindings(m_stmt);
	sqlite3_reset(m_stmt);
}

StormByte::Database::ExpectedRows PreparedSTMT::DoExecute() {
	Rows rows;
	while (sqlite3_step(m_stmt) == SQLITE_ROW) {
		Row row;
		for (auto i = 0; i < sqlite3_column_count(m_stmt); i++) {
			std::string column_name = sqlite3_column_name(m_stmt, i);
			switch(sqlite3_column_type(m_stmt, i)) {
				case SQLITE_INTEGER:
					if (sqlite3_column_int(m_stmt, i) > std::numeric_limits<int>::max())
						row.Add(std::move(column_name), (long int)sqlite3_column_int64(m_stmt, i));
					else
					row.Add(std::move(column_name), (int)sqlite3_column_int(m_stmt, i));
					break;
				case SQLITE_TEXT:
					row.Add(std::move(column_name), std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, i))));
					break;
				case SQLITE_FLOAT:
					row.Add(std::move(column_name), sqlite3_column_double(m_stmt, i));
					break;
				case SQLITE_BLOB: {
					const std::byte* blobData = reinterpret_cast<const std::byte*>(sqlite3_column_blob(m_stmt, i));
					int blobSize = sqlite3_column_bytes(m_stmt, i);
					std::vector<std::byte> blobVec(blobData, blobData + blobSize);
					row.Add(std::move(column_name), std::move(blobVec));
					break;
				}
				case SQLITE_NULL:
				default:
					row.Add(std::move(column_name), nullptr);
					break;
			}
		}
		rows.Add(std::move(row));
	}
	return rows;
}

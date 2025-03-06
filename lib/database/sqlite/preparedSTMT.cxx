#include <database/sqlite/preparedSTMT.hxx>
#include <database/row.hxx>

#include <limits>
#include <sqlite3.h>

using namespace StormByte::Database::SQLite;

PreparedSTMT::PreparedSTMT(const std::string& name, const std::string& query):Database::PreparedSTMT<Row>(name, query), m_stmt(nullptr) {}

PreparedSTMT::PreparedSTMT(std::string&& name, std::string&& query) noexcept:Database::PreparedSTMT<Row>(std::move(name), std::move(query)), m_stmt(nullptr) {}

PreparedSTMT::~PreparedSTMT() noexcept {
	if (m_stmt) {
		sqlite3_finalize(m_stmt);
		m_stmt = nullptr;
	}
}

PreparedSTMT& PreparedSTMT::Bind(const int& column, const std::nullptr_t&) noexcept {
	sqlite3_bind_null(m_stmt, column + 1);
	return *this;
}

PreparedSTMT& PreparedSTMT::Bind(const int& column, const int& val) noexcept {
	sqlite3_bind_int(m_stmt, column + 1, val);
	return *this;
}

PreparedSTMT& PreparedSTMT::Bind(const int& column, const unsigned int& val) noexcept {
	sqlite3_bind_int(m_stmt, column + 1, val);
	return *this;
}

PreparedSTMT& PreparedSTMT::Bind(const int& column, const int64_t& val) noexcept {
	sqlite3_bind_int64(m_stmt, column + 1, val);
	return *this;
}

PreparedSTMT& PreparedSTMT::Bind(const int& column, const uint64_t& val) noexcept {
	sqlite3_bind_int64(m_stmt, column + 1, val);
	return *this;
}

PreparedSTMT& PreparedSTMT::Bind(const int& column, const double& val) noexcept {
	sqlite3_bind_double(m_stmt, column + 1, val);
	return *this;
}

PreparedSTMT& PreparedSTMT::Bind(const int& column, bool val) noexcept {
	sqlite3_bind_int(m_stmt, column + 1, val);
	return *this;
}

PreparedSTMT& PreparedSTMT::Bind(const int& column, const std::string& val) noexcept {
	sqlite3_bind_text(m_stmt, column + 1, val.c_str(), -1, SQLITE_STATIC);
	return *this;
}

void PreparedSTMT::Reset() noexcept {
	sqlite3_clear_bindings(m_stmt);
	sqlite3_reset(m_stmt);
}

const Row& PreparedSTMT::Step() noexcept {
	m_row = std::make_unique<Row>();
	if (sqlite3_step(m_stmt) == SQLITE_ROW) {
		for (auto i = 0; i < sqlite3_column_count(m_stmt); i++) {
			const std::string column_name = sqlite3_column_name(m_stmt, i);
			switch(sqlite3_column_type(m_stmt, i)) {
				case SQLITE_INTEGER:
					if (sqlite3_column_int(m_stmt, i) > std::numeric_limits<int>::max())
						m_row->Add(column_name, std::make_unique<Value>((int64_t)sqlite3_column_int64(m_stmt, i)));
					else
					m_row->Add(column_name, std::make_unique<Value>((int)sqlite3_column_int(m_stmt, i)));
					break;

				case SQLITE_TEXT:
					m_row->Add(column_name, std::make_unique<Value>(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, i)))));
					break;

				case SQLITE_NULL:
					m_row->Add(column_name, std::make_unique<Value>(nullptr));
					break;

				case SQLITE_FLOAT:
					m_row->Add(column_name, std::make_unique<Value>(sqlite3_column_double(m_stmt, i)));
					break;

				default:
					/* Not implemented all the cases */
					break;
			}
		}
	}
	
	return *m_row;
}
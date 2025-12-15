#include <StormByte/database/postgres/prepared_stmt.hxx>
#include <StormByte/database/postgres/result_fetch.hxx>
#include <libpq-fe.h>

using namespace StormByte::Database::Postgres;

PreparedSTMT::PreparedSTMT(const std::string& name, const std::string& query):Database::PreparedSTMT(name, query), m_conn(nullptr), m_stmt_name(name) {}

PreparedSTMT::PreparedSTMT(std::string&& name, std::string&& query) noexcept:Database::PreparedSTMT(std::move(name), std::move(query)), m_conn(nullptr), m_stmt_name(Database::PreparedSTMT::m_name) {}

void PreparedSTMT::Bind(const int& index, const std::nullptr_t&) noexcept {
	if (static_cast<std::size_t>(index) >= m_param_values.size()) {
		m_param_values.resize(index + 1, nullptr);
		m_param_lengths.resize(index + 1, 0);
		m_param_formats.resize(index + 1, 0);
	}
	m_param_values[index] = nullptr;
	m_param_lengths[index] = 0;
	m_param_formats[index] = 0;
}

void PreparedSTMT::Bind(const int& index, const int& val) noexcept {
	if (static_cast<std::size_t>(index) >= m_param_values.size()) {
		m_param_values.resize(index + 1, nullptr);
		m_param_lengths.resize(index + 1, 0);
		m_param_formats.resize(index + 1, 0);
	}
	m_string_storage.emplace_back(std::to_string(val));
	m_param_values[index] = m_string_storage.back().c_str();
	m_param_lengths[index] = 0;
	m_param_formats[index] = 0;
}

void PreparedSTMT::Bind(const int& index, const unsigned int& val) noexcept {
	Bind(index, static_cast<int>(val));
}

void PreparedSTMT::Bind(const int& index, const int64_t& val) noexcept {
	if (static_cast<std::size_t>(index) >= m_param_values.size()) {
		m_param_values.resize(index + 1, nullptr);
		m_param_lengths.resize(index + 1, 0);
		m_param_formats.resize(index + 1, 0);
	}
	m_string_storage.emplace_back(std::to_string(val));
	m_param_values[index] = m_string_storage.back().c_str();
	m_param_lengths[index] = 0;
	m_param_formats[index] = 0;
}

void PreparedSTMT::Bind(const int& index, const uint64_t& val) noexcept {
	Bind(index, static_cast<int64_t>(val));
}

void PreparedSTMT::Bind(const int& index, const double& val) noexcept {
	if (static_cast<std::size_t>(index) >= m_param_values.size()) {
		m_param_values.resize(index + 1, nullptr);
		m_param_lengths.resize(index + 1, 0);
		m_param_formats.resize(index + 1, 0);
	}
	m_string_storage.emplace_back(std::to_string(val));
	m_param_values[index] = m_string_storage.back().c_str();
	m_param_lengths[index] = 0;
	m_param_formats[index] = 0;
}

void PreparedSTMT::Bind(const int& index, bool val) noexcept {
	Bind(index, static_cast<int>(val));
}

void PreparedSTMT::Bind(const int& index, const std::string& val) noexcept {
	if (static_cast<std::size_t>(index) >= m_param_values.size()) {
		m_param_values.resize(index + 1, nullptr);
		m_param_lengths.resize(index + 1, 0);
		m_param_formats.resize(index + 1, 0);
	}
	m_string_storage.emplace_back(val);
	m_param_values[index] = m_string_storage.back().c_str();
	m_param_lengths[index] = 0;
	m_param_formats[index] = 0;
}

void PreparedSTMT::Bind(const int& index, const std::vector<std::byte>& val) noexcept {
	if (static_cast<std::size_t>(index) >= m_param_values.size()) {
		m_param_values.resize(index + 1, nullptr);
		m_param_lengths.resize(index + 1, 0);
		m_param_formats.resize(index + 1, 0);
	}
	m_blob_storage.emplace_back();
	m_blob_storage.back().resize(val.size());
	for (std::size_t i = 0; i < val.size(); ++i)
		m_blob_storage.back()[i] = static_cast<char>(val[i]);
	m_param_values[index] = m_blob_storage.back().data();
	m_param_lengths[index] = static_cast<int>(m_blob_storage.back().size());
	m_param_formats[index] = 1; // binary
}

void PreparedSTMT::Reset() noexcept {
	m_param_values.clear();
	m_param_lengths.clear();
	m_param_formats.clear();
	m_string_storage.clear();
	m_blob_storage.clear();
}

StormByte::Database::ExpectedRows PreparedSTMT::DoExecute() {
	if (!m_conn)
		return Unexpected<ExecuteError>("No connection available for prepared statement");

	int nParams = static_cast<int>(m_param_values.size());
	std::vector<const char*> params(nParams);
	std::vector<int> lengths(nParams);
	std::vector<int> formats(nParams);

	for (int i = 0; i < nParams; ++i) {
		params[i] = m_param_values[i];
		lengths[i] = m_param_lengths[i];
		formats[i] = m_param_formats[i];
	}

	PGresult* res = PQexecPrepared(m_conn, m_stmt_name.c_str(), nParams, params.data(), lengths.data(), formats.data(), 0);
	if (!res) {
		return Unexpected<ExecuteError>("Null PGresult from PQexecPrepared");
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

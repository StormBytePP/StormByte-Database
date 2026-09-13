/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte-Database.
 *
 * StormByte-Database is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * or later, as published by the Free Software Foundation.
 *
 * StormByte-Database is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with StormByte-Database. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 */

#include <StormByte/database/postgres/prepared_stmt.hxx>
#include <StormByte/database/postgres/result_fetch.hxx>
#include <libpq-fe.h>
using namespace StormByte::Database::Postgres;
PreparedSTMT::PreparedSTMT(const std::string& name, const std::string& query, std::shared_ptr<Logger::Log> logger)
	: Database::PreparedSTMT(name, query, std::move(logger)), m_conn(nullptr), m_stmt_name(name) {}
PreparedSTMT::PreparedSTMT(std::string&& name, std::string&& query, std::shared_ptr<Logger::Log> logger) noexcept
	: Database::PreparedSTMT(std::move(name), std::move(query), std::move(logger)), m_conn(nullptr), m_stmt_name(Database::PreparedSTMT::m_name) {}
void PreparedSTMT::Binder(const int& index, Value&& value) noexcept {
	if (static_cast<std::size_t>(index) >= m_params.size())
		m_params.resize(index + 1);
	m_params[index] = std::move(value);
}
void PreparedSTMT::Reset() noexcept {
	m_params.clear();
}
StormByte::Database::ExpectedRows PreparedSTMT::DoExecute() {
	if (!m_conn)
		return Unexpected<ExecuteError>("No connection available for prepared statement");
	int nParams = static_cast<int>(m_params.size());
	std::vector<const char*> params(nParams);
	std::vector<int> lengths(nParams);
	std::vector<int> formats(nParams);
	std::vector<std::string> string_storage(nParams);
	std::vector<std::vector<char>> blob_storage(nParams);
	for (int i = 0; i < nParams; ++i) {
		const Value& value = m_params[i];
		if (value.IsNull())
			continue;
		switch (value.Type()) {
			case Value::Type::Integer:
				string_storage[i] = std::to_string(value.Get<int>());
				break;
			case Value::Type::UnsignedInteger:
				string_storage[i] = std::to_string(value.Get<unsigned int>());
				break;
			case Value::Type::LongInteger:
				string_storage[i] = std::to_string(value.Get<long int>());
				break;
			case Value::Type::UnsignedLongInteger:
				string_storage[i] = std::to_string(value.Get<unsigned long int>());
				break;
			case Value::Type::Double:
				string_storage[i] = std::to_string(value.Get<double>());
				break;
			case Value::Type::Boolean:
				string_storage[i] = value.Get<bool>() ? "true" : "false";
				break;
			case Value::Type::Text:
				string_storage[i] = value.Get<std::string>();
				break;
			case Value::Type::Blob: {
				auto bytes = value.Get<std::vector<std::byte>>();
				blob_storage[i].resize(bytes.size());
				for (std::size_t byte_index = 0; byte_index < bytes.size(); ++byte_index)
					blob_storage[i][byte_index] = static_cast<char>(bytes[byte_index]);
				params[i] = blob_storage[i].data();
				lengths[i] = static_cast<int>(blob_storage[i].size());
				formats[i] = 1;
				continue;
			}
			case Value::Type::Null:
			default:
				continue;
		}
		params[i] = string_storage[i].c_str();
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

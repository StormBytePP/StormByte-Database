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

#include <StormByte/database/sqlite/prepared_stmt.hxx>
#include <StormByte/database/sqlite/result_fetch.hxx>
using namespace StormByte::Database::SQLite;
PreparedSTMT::PreparedSTMT(const std::string& name, const std::string& query, std::shared_ptr<Logger::Log> logger)
	: Database::PreparedSTMT(name, query, std::move(logger)), m_stmt(nullptr) {}
PreparedSTMT::PreparedSTMT(std::string&& name, std::string&& query, std::shared_ptr<Logger::Log> logger) noexcept
	: Database::PreparedSTMT(std::move(name), std::move(query), std::move(logger)), m_stmt(nullptr) {}
PreparedSTMT::~PreparedSTMT() noexcept {
	if (m_stmt) {
		sqlite3_finalize(m_stmt);
		m_stmt = nullptr;
	}
}
void PreparedSTMT::Binder(const int& index, Value&& value) noexcept {
	if (!m_stmt) return;
	const int col = index + 1;
	if (value.IsNull()) {
		sqlite3_bind_null(m_stmt, col);
		return;
	}
	switch (value.Type()) {
		case Value::Type::Integer:
			sqlite3_bind_int(m_stmt, col, value.Get<int>());
			break;
		case Value::Type::UnsignedInteger:
			sqlite3_bind_int(m_stmt, col, static_cast<int>(value.Get<unsigned int>()));
			break;
		case Value::Type::LongInteger:
			sqlite3_bind_int64(m_stmt, col, value.Get<long int>());
			break;
		case Value::Type::UnsignedLongInteger:
			sqlite3_bind_int64(m_stmt, col, static_cast<sqlite3_int64>(value.Get<unsigned long int>()));
			break;
		case Value::Type::Double:
			sqlite3_bind_double(m_stmt, col, value.Get<double>());
			break;
		case Value::Type::Boolean:
			sqlite3_bind_int(m_stmt, col, value.Get<bool>() ? 1 : 0);
			break;
		case Value::Type::Text: {
			const std::string& s = value.Get<std::string>();
			sqlite3_bind_text(m_stmt, col, s.c_str(), -1, SQLITE_TRANSIENT);
			break;
		}
		case Value::Type::Blob: {
			auto bv = value.Get<std::vector<std::byte>>();
			if (bv.empty()) {
				sqlite3_bind_blob(m_stmt, col, nullptr, 0, SQLITE_TRANSIENT);
			} else {
				sqlite3_bind_blob(m_stmt, col, reinterpret_cast<const void*>(bv.data()), static_cast<int>(bv.size()), SQLITE_TRANSIENT);
			}
			break;
		}
		default:
			sqlite3_bind_null(m_stmt, col);
			break;
	}
}
void PreparedSTMT::Reset() noexcept {
	if (m_stmt) {
		sqlite3_clear_bindings(m_stmt);
		sqlite3_reset(m_stmt);
	}
}
StormByte::Database::ExpectedRows PreparedSTMT::DoExecute() {
	return StepResults(m_stmt);
}

/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte-Database.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */

#include <StormByte/database/mssql/mssql.hxx>
#include <StormByte/database/mssql/prepared_stmt.hxx>
#include <sybdb.h>

#include <limits>
#include <utility>

using namespace StormByte::Database::MSSQL;

PreparedSTMT::PreparedSTMT(ConstructionKey, const std::string_view name, const std::string_view query,
		struct tds_dblib_dbprocess* const connection,
		const StormByte::Safe::Shared<Logger::Log>& logger,
		const StormByte::Safe::Shared<StormByte::Database::Telemetry>& telemetry):
	StormByte::Database::PreparedSTMT(name, query, logger, telemetry), m_connection(connection),
	m_bind_error(false) {}

PreparedSTMT::PreparedSTMT(PreparedSTMT&& other) noexcept:
	StormByte::Database::PreparedSTMT(std::move(other)),
	m_connection(std::exchange(other.m_connection, nullptr)),
	m_parameters(std::move(other.m_parameters)), m_bind_error(other.m_bind_error) {}

PreparedSTMT::~PreparedSTMT() noexcept = default;

PreparedSTMT& PreparedSTMT::operator=(PreparedSTMT&& other) noexcept {
	if (this != &other) {
		StormByte::Database::PreparedSTMT::operator=(std::move(other));
		m_connection = std::exchange(other.m_connection, nullptr);
		m_parameters = std::move(other.m_parameters);
		m_bind_error = other.m_bind_error;
	}
	return *this;
}

void PreparedSTMT::Binder(const StormByte::Size index, Value&& value) noexcept {
	if (index >= StormByte::Size{std::numeric_limits<int>::max()}) {
		m_bind_error = true;
		return;
	}
	try {
		const std::size_t position = static_cast<std::size_t>(index);
		if (position >= m_parameters.size())
			m_parameters.resize(position + 1);
		m_parameters[position] = std::move(value);
	} catch (...) {
		m_bind_error = true;
	}
}

void PreparedSTMT::Reset() noexcept {
	m_parameters.clear();
	m_bind_error = false;
}

StormByte::Database::ExpectedRows PreparedSTMT::DoExecute() {
	if (m_bind_error)
		return StormByte::Unexpected<QueryException>(ExecuteError("MSSQL prepared statement has invalid bindings"));
	if (!m_connection)
		return StormByte::Unexpected<QueryException>(ExecuteError("MSSQL prepared statement has no connection"));
	auto* owner = reinterpret_cast<MSSQL*>(dbgetuserdata(m_connection));
	if (!owner)
		return StormByte::Unexpected<QueryException>(ExecuteError("MSSQL prepared statement connection is no longer available"));
	return owner->ExecuteParameterized(Query(), m_parameters);
}
/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte-Database.
 *
 * StormByte-Database original source is dual-licensed:
 *
 * 1. GNU Lesser General Public License v3.0 (or later)
 *    You may redistribute and/or modify this file under the terms of the
 *    GNU Lesser General Public License as published by the Free Software
 *    Foundation, either version 3 of the License, or (at your option)
 *    any later version.
 *
 * 2. Commercial license
 *    Alternatively, this file may be used under the terms of a commercial
 *    license agreement with the copyright holder
 *    (David C. Manuelda <StormByte@gmail.com>).
 *
 * Both licenses apply only to original StormByte-Database source in this
 * repository. They do not cover other StormByte modules or any third-party
 * material shipped with this repository (including everything under
 * thirdparty/, and in particular the bundled StormByte-Logger tree and
 * the PostgreSQL, MariaDB and SQLite trees), which remain under their own
 * licenses.
 *
 * Neither license grants any patent rights. Any patent licenses required
 * to use this software or third-party components must be obtained separately
 * from the patent holders.
 *
 * StormByte-Database is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with StormByte-Database. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */

#include <StormByte/database/database.hxx>
#include <exception>
#include <string_view>

using namespace StormByte::Database;

Database::Database(const StormByte::Shared<Logger::Log>& logger):
	m_operation_mutex(StormByte::Shared<std::recursive_mutex>::MakePointer<std::recursive_mutex>()),
	m_connected(false), m_ssl_mode(SslMode::Default), m_logger(logger) {}

Database::Database(Database&& other) noexcept:
	m_operation_mutex(other.m_operation_mutex),
	m_connected(false), m_ssl_mode(SslMode::Default) {
	std::lock_guard<std::recursive_mutex> lock(*other.m_operation_mutex);
	m_logger = other.m_logger;
	m_connected = std::exchange(other.m_connected, false);
	m_ssl_mode = other.m_ssl_mode;
	m_prepared_stmts = std::move(other.m_prepared_stmts);
}

Database& Database::operator=(Database&& other) noexcept {
	if (this != &other) {
		auto transfer = [this, &other]() {
			ClearPreparedSTMTs();
			m_logger = other.m_logger;
			m_connected = std::exchange(other.m_connected, false);
			m_ssl_mode = other.m_ssl_mode;
			m_prepared_stmts = std::move(other.m_prepared_stmts);
		};
		if (m_operation_mutex == other.m_operation_mutex) {
			std::lock_guard<std::recursive_mutex> lock(*m_operation_mutex);
			transfer();
		} else {
			std::scoped_lock lock(*m_operation_mutex, *other.m_operation_mutex);
			transfer();
		}
	}
	return *this;
}

Database::~Database() noexcept = default;

void Database::ClearPreparedSTMTs() noexcept {
	std::lock_guard<std::recursive_mutex> lock(*m_operation_mutex);
	m_prepared_stmts.clear();
}

PreparedSTMT* Database::FindPreparedSTMT(std::string_view name) {
	auto it = m_prepared_stmts.find(std::string{name});
	return it == m_prepared_stmts.end() ? nullptr : it->second.get();
}
bool Database::Connect() noexcept {
	std::lock_guard<std::recursive_mutex> lock(*m_operation_mutex);
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Connect enter" << std::endl;
	DoPreConnect();
	bool result = DoConnect();
	if (result) {
		m_connected = true;
		DoPostConnect();
	}

	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Connect leave (" << (result ? "ok" : "fail") << ")" << std::endl;
	return result;
}

void Database::Disconnect() noexcept {
	std::lock_guard<std::recursive_mutex> lock(*m_operation_mutex);
	if (!m_connected)
		return;
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Disconnect enter" << std::endl;
	DoPreDisconnect();
	DoDisconnect();
	DoPostDisconnect();
	m_connected = false;
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Disconnect leave" << std::endl;
}

void Database::PrepareSTMT(std::string_view name, std::string_view query) noexcept {
	DoPrepareSTMT(name, query);
}

void Database::DoPrepareSTMT(std::string_view name, std::string_view query) noexcept {
	std::lock_guard<std::recursive_mutex> lock(*m_operation_mutex);
	if (m_logger)
		*m_logger << Logger::Level::Debug << "Preparing statement '" << name << "': " << query << std::endl;
	StormByte::Unique<PreparedSTMT> prepared = CreatePreparedSTMT(name, query);
	if (prepared)
		m_prepared_stmts.emplace(std::string{prepared->Name()}, std::move(prepared));
}

StormByte::Expected<Transaction, TransactionError> Database::BeginTransaction(IsolationLevel level) {
	bool begun = false;
	std::unique_lock<std::recursive_mutex> lock;
	try {
		lock = std::unique_lock<std::recursive_mutex>(*m_operation_mutex);
		if (m_logger)
			*m_logger << Logger::Level::Debug << "BeginTransaction" << std::endl;
		DoBeginTransaction(level);
		begun = true;
		return Transaction(*this);
	} catch (const std::exception& error) {
		if (begun && lock.owns_lock())
			DoSilentQuery("ROLLBACK;");
		return Unexpected<TransactionError>(error.what());
	} catch (...) {
		if (begun && lock.owns_lock())
			DoSilentQuery("ROLLBACK;");
		return Unexpected<TransactionError>("Unknown backend failure");
	}
}

void Database::CommitTransaction() {
	std::lock_guard<std::recursive_mutex> lock(*m_operation_mutex);
	if (m_logger)
		*m_logger << Logger::Level::Debug << "CommitTransaction" << std::endl;
	if (!DoSilentQuery("COMMIT;"))
		throw ExecuteError("Unable to commit transaction.");
}

void Database::RollbackTransaction() {
	std::lock_guard<std::recursive_mutex> lock(*m_operation_mutex);
	if (m_logger)
		*m_logger << Logger::Level::Debug << "RollbackTransaction" << std::endl;
	DoSilentQuery("ROLLBACK;");
}

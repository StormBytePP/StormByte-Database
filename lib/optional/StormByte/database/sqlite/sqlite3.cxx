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

#include <StormByte/database/sqlite/sqlite3.hxx>
#include <StormByte/database/sqlite/result_fetch.hxx>
#include <StormByte/database/sqlite/prepared_stmt.hxx>
#include <sqlite3.h>
#include <atomic>
#include <mutex>
using namespace StormByte::Database::SQLite;
namespace {
	std::atomic<int> g_sqlite_refcount{0};
	std::mutex g_sqlite_init_mutex;
}
SQLite3::SQLite3(std::shared_ptr<Logger::Log> logger) noexcept
	: SQLite3(":memory:", logger) {}
SQLite3::SQLite3(const std::filesystem::path& dbfile, std::shared_ptr<Logger::Log> logger)
	: Database(logger), m_database_file(dbfile), m_database(nullptr) {}
SQLite3::SQLite3(std::filesystem::path&& dbfile, std::shared_ptr<Logger::Log>&& logger)
	: Database(std::move(logger)), m_database_file(std::move(dbfile)), m_database(nullptr) {}
SQLite3::~SQLite3() noexcept {
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "SQLite3 dtor" << std::endl;
	Disconnect();
}
bool SQLite3::DoConnect() noexcept {
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "SQLite3::DoConnect enter" << std::endl;
	if (m_connected)
		return false;
	{
		std::lock_guard<std::mutex> lock(g_sqlite_init_mutex);
		if (g_sqlite_refcount == 0) {
			if (sqlite3_initialize() != SQLITE_OK) {
				if (m_logger)
					*m_logger << Logger::Level::Error << "sqlite3_initialize failed" << std::endl;
				return false;
			}
		}
		++g_sqlite_refcount;
	}
	if (sqlite3_open(m_database_file.string().c_str(), &m_database) != SQLITE_OK) {
		if (m_logger) {
			*m_logger << Logger::Level::Error << "sqlite3_open failed: "
					<< (m_database ? sqlite3_errmsg(m_database) : "unknown") << std::endl;
		}
		if (m_database) {
			sqlite3_close(m_database);
			m_database = nullptr;
		}
		std::lock_guard<std::mutex> lock(g_sqlite_init_mutex);
		if (--g_sqlite_refcount == 0)
			sqlite3_shutdown();
		return false;
	}
	sqlite3_busy_timeout(m_database, 30000);
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "SQLite3::DoConnect leave (ok)" << std::endl;
	return true;
}
void SQLite3::DoPreDisconnect() noexcept {
	if (m_database)
		m_prepared_stmts.clear();
}
void SQLite3::DoDisconnect() noexcept {
	if (m_database) {
		sqlite3_close(m_database);
		m_database = nullptr;
	}
}
void SQLite3::DoPostDisconnect() noexcept {
	std::lock_guard<std::mutex> lock(g_sqlite_init_mutex);
	if (g_sqlite_refcount > 0) {
		if (--g_sqlite_refcount == 0)
			sqlite3_shutdown();
	}
}
StormByte::Database::ExpectedRows SQLite3::Query(const std::string& query) noexcept {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "Executing query: " << query << std::endl;
	if (!m_connected)
		return Unexpected<ExecuteError>("Database not connected");
	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_database, query.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		const std::string errorStr = sqlite3_errmsg(m_database);
		if (stmt)
			sqlite3_finalize(stmt);
		return Unexpected<ExecuteError>(errorStr);
	}
	ExpectedRows result = StepResults(stmt);
	sqlite3_finalize(stmt);
	return result;
}
bool SQLite3::SilentQuery(const std::string& query) noexcept {
	return DoSilentQuery(query);
}
bool SQLite3::DoSilentQuery(const std::string& query) noexcept {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "Executing silent query: " << query << std::endl;
	if (!m_connected)
		return false;
	char* errMsg = nullptr;
	int rc = sqlite3_exec(m_database, query.c_str(), nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK) {
		if (errMsg) {
			if (m_logger) {
				*m_logger << Logger::Level::Error
						<< "SQLite3 SilentQuery error: " << errMsg << std::endl;
			}
			sqlite3_free(errMsg);
		}
		return false;
	}
	return true;
}
void SQLite3::EnableForeignKeys() {
	DoSilentQuery("PRAGMA foreign_keys = ON;");
}
std::unique_ptr<StormByte::Database::PreparedSTMT>
SQLite3::CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept {
	std::unique_ptr<PreparedSTMT> stmt =
		std::make_unique<PreparedSTMT>(PreparedSTMT(std::move(name), std::move(query), m_logger));
	sqlite3_prepare_v2(m_database, stmt->Query().c_str(),
					static_cast<int>(stmt->Query().length()),
					&(stmt->m_stmt), nullptr);
	if (!stmt->m_stmt) {
		if (m_logger)
			*m_logger << Logger::Level::Error << "Failed to prepare statement" << std::endl;
		return nullptr;
	}
	return stmt;
}
void SQLite3::DoBeginTransaction(IsolationLevel level) {
	switch (level) {
		case IsolationLevel::ReadUncommitted:
		case IsolationLevel::ReadCommitted:
		case IsolationLevel::Default:
			DoSilentQuery("BEGIN DEFERRED;");
			break;
		case IsolationLevel::RepeatableRead:
			DoSilentQuery("BEGIN IMMEDIATE;");
			break;
		case IsolationLevel::Serializable:
			DoSilentQuery("BEGIN EXCLUSIVE;");
			break;
	}
}

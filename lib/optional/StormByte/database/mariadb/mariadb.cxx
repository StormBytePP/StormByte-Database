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

#include <StormByte/database/mariadb/mariadb.hxx>
#include <StormByte/database/mariadb/result_fetch.hxx>
#include <StormByte/database/mariadb/prepared_stmt.hxx>
#include <mysql.h>
#include <string>
using namespace StormByte::Database::MariaDB;
namespace {
	void LogMariaDBWarnings(MYSQL* conn, std::shared_ptr<StormByte::Logger::Log>& logger) {
		if (!conn || !logger)
			return;
		const unsigned int count = mysql_warning_count(conn);
		if (count == 0)
			return;
		if (mysql_query(conn, "SHOW WARNINGS") != 0)
			return;
		MYSQL_RES* res = mysql_store_result(conn);
		if (!res)
			return;
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(res)) != nullptr) {
			const char* level = row[0] ? row[0] : "";
			const char* code  = row[1] ? row[1] : "";
			const char* msg   = row[2] ? row[2] : "";
			*logger << StormByte::Logger::Level::Notice
					<< "MariaDB " << level << " (" << code << "): " << msg << std::endl;
		}
		mysql_free_result(res);
	}
	void ApplySslMode(MYSQL* conn, StormByte::Database::SslMode mode) {
		if (!conn)
			return;
#if defined(MYSQL_OPT_SSL_MODE)
		int ssl_mode = 0;
		switch (mode) {
			case StormByte::Database::SslMode::Disable:
#if defined(SSL_MODE_DISABLED)
				ssl_mode = SSL_MODE_DISABLED;
				mysql_options(conn, MYSQL_OPT_SSL_MODE, &ssl_mode);
#endif
				break;
			case StormByte::Database::SslMode::Require:
#if defined(SSL_MODE_REQUIRED)
				ssl_mode = SSL_MODE_REQUIRED;
				mysql_options(conn, MYSQL_OPT_SSL_MODE, &ssl_mode);
#endif
				break;
			case StormByte::Database::SslMode::Prefer:
#if defined(SSL_MODE_PREFERRED)
				ssl_mode = SSL_MODE_PREFERRED;
				mysql_options(conn, MYSQL_OPT_SSL_MODE, &ssl_mode);
#endif
				break;
			case StormByte::Database::SslMode::Default:
			default:
				break;
		}
#elif defined(MYSQL_OPT_SSL_ENFORCE)
		if (mode == StormByte::Database::SslMode::Disable) {
			my_bool enforce = 0;
			mysql_options(conn, MYSQL_OPT_SSL_ENFORCE, &enforce);
		} else if (mode == StormByte::Database::SslMode::Require) {
			my_bool enforce = 1;
			mysql_options(conn, MYSQL_OPT_SSL_ENFORCE, &enforce);
		}
#else
		(void)mode;
#endif
	}
}
MariaDB::~MariaDB() noexcept {
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "MariaDB dtor" << std::endl;
	Disconnect();
}
MariaDB::MariaDB(const std::string& host, const std::string& user, const std::string& password,
				const std::string& db_name, int port, std::shared_ptr<Logger::Log> logger)
	: Database(logger), m_host(host), m_user(user), m_password(password),
	m_dbname(db_name), m_port(port), m_conn(nullptr) {}
MariaDB::MariaDB(std::string&& host, std::string&& user, std::string&& password,
				std::string&& db_name, int port, std::shared_ptr<Logger::Log> logger)
	: Database(logger), m_host(std::move(host)), m_user(std::move(user)),
	m_password(std::move(password)), m_dbname(std::move(db_name)),
	m_port(port), m_conn(nullptr) {}
bool MariaDB::DoConnect() noexcept {
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "MariaDB::DoConnect enter" << std::endl;
	if (m_connected)
		return false;
	MYSQL* conn = mysql_init(nullptr);
	if (!conn) {
		if (m_logger)
			*m_logger << Logger::Level::Error << "mysql_init failed" << std::endl;
		return false;
	}
	ApplySslMode(conn, m_ssl_mode);
	unsigned int port = static_cast<unsigned int>(m_port);
	if (!mysql_real_connect(conn,
							m_host.empty() ? nullptr : m_host.c_str(),
							m_user.empty() ? nullptr : m_user.c_str(),
							m_password.empty() ? nullptr : m_password.c_str(),
							m_dbname.empty() ? nullptr : m_dbname.c_str(),
							port, nullptr, 0)) {
		if (m_logger) {
			*m_logger << Logger::Level::Error
					<< "MariaDB connection error: "
					<< (mysql_error(conn) ? mysql_error(conn) : "Unknown error")
					<< std::endl;
		}
		mysql_close(conn);
		m_conn = nullptr;
		return false;
	}
	m_conn = conn;
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "MariaDB::DoConnect leave (ok)" << std::endl;
	return true;
}
void MariaDB::DoPreDisconnect() noexcept {
	m_prepared_stmts.clear();
}
void MariaDB::DoDisconnect() noexcept {
	if (m_conn) {
		mysql_close(m_conn);
		m_conn = nullptr;
	}
}
StormByte::Database::ExpectedRows MariaDB::Query(const std::string& query) noexcept {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "Executing query: " << query << std::endl;
	if (!m_connected || !m_conn)
		return Unexpected<ExecuteError>("Database not connected");
	if (mysql_real_query(m_conn, query.c_str(), static_cast<unsigned long>(query.size())) != 0) {
		return Unexpected<ExecuteError>(mysql_error(m_conn) ? mysql_error(m_conn) : "Unknown MySQL error");
	}
	LogMariaDBWarnings(m_conn, m_logger);
	MYSQL_RES* res = mysql_store_result(m_conn);
	if (!res) {
		if (mysql_field_count(m_conn) == 0)
			return Rows();
		return Unexpected<ExecuteError>(mysql_error(m_conn) ? mysql_error(m_conn) : "Unknown MySQL error");
	}
	StormByte::Database::ExpectedRows rows = StormByte::Database::MariaDB::StepResults(res);
	mysql_free_result(res);
	return rows;
}
bool MariaDB::SilentQuery(const std::string& query) noexcept {
	return DoSilentQuery(query);
}
bool MariaDB::DoSilentQuery(const std::string& query) noexcept {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "Executing silent query: " << query << std::endl;
	if (!m_connected || !m_conn)
		return false;
	if (mysql_real_query(m_conn, query.c_str(), static_cast<unsigned long>(query.size())) != 0) {
		if (m_logger) {
			*m_logger << Logger::Level::Error
					<< "MariaDB SilentQuery error: "
					<< (mysql_error(m_conn) ? mysql_error(m_conn) : "Unknown MySQL error")
					<< std::endl;
		}
		return false;
	}
	LogMariaDBWarnings(m_conn, m_logger);
	return true;
}
std::unique_ptr<StormByte::Database::PreparedSTMT>
MariaDB::CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept {
	if (!m_conn)
		return nullptr;
	return std::unique_ptr<PreparedSTMT>(
		new PreparedSTMT(std::move(name), std::move(query), m_conn, m_logger));
}
void MariaDB::DoBeginTransaction(IsolationLevel level) {
	switch (level) {
		case IsolationLevel::ReadUncommitted:
			DoSilentQuery("SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;");
			break;
		case IsolationLevel::ReadCommitted:
			DoSilentQuery("SET TRANSACTION ISOLATION LEVEL READ COMMITTED;");
			break;
		case IsolationLevel::RepeatableRead:
			DoSilentQuery("SET TRANSACTION ISOLATION LEVEL REPEATABLE READ;");
			break;
		case IsolationLevel::Serializable:
			DoSilentQuery("SET TRANSACTION ISOLATION LEVEL SERIALIZABLE;");
			break;
		case IsolationLevel::Default:
		default:
			break;
	}
	DoSilentQuery("BEGIN;");
}

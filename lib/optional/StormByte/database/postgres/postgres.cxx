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

#include <StormByte/database/postgres/postgres.hxx>
#include <StormByte/database/postgres/result_fetch.hxx>
#include <StormByte/database/postgres/prepared_stmt.hxx>
#include <libpq-fe.h>
#include <cctype>
#include <string>
using namespace StormByte::Database::Postgres;
namespace {
	void PostgresNoticeProcessor(void* arg, const char* message) {
		auto* log = static_cast<StormByte::Logger::Log*>(arg);
		if (!log || !message)
			return;
		std::string msg(message);
		while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r'))
			msg.pop_back();
		if (!msg.empty())
			*log << StormByte::Logger::Level::Notice << msg << std::endl;
	}
}
Postgres::Postgres(const std::string& host, const std::string& user, const std::string& password,
				const std::string& db_name, std::shared_ptr<Logger::Log> logger)
	: Database(logger), m_host(host), m_user(user), m_password(password),
	m_dbname(db_name), m_conn(nullptr) {}
Postgres::Postgres(std::string&& host, std::string&& user, std::string&& password,
				std::string&& db_name, std::shared_ptr<Logger::Log> logger)
	: Database(logger), m_host(std::move(host)), m_user(std::move(user)),
	m_password(std::move(password)), m_dbname(std::move(db_name)), m_conn(nullptr) {}
Postgres::~Postgres() noexcept {
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Postgres dtor" << std::endl;
	Disconnect();
}
bool Postgres::DoConnect() noexcept {
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Postgres::DoConnect enter" << std::endl;
	if (m_connected)
		return false;
	std::string conninfo;
	if (!m_host.empty())     conninfo += "host='" + m_host + "' ";
	if (!m_user.empty())     conninfo += "user='" + m_user + "' ";
	if (!m_password.empty()) conninfo += "password='" + m_password + "' ";
	if (!m_dbname.empty())   conninfo += "dbname='" + m_dbname + "' ";
	switch (m_ssl_mode) {
		case SslMode::Disable:
			conninfo += "sslmode=disable ";
			break;
		case SslMode::Prefer:
			conninfo += "sslmode=prefer ";
			break;
		case SslMode::Require:
			conninfo += "sslmode=require ";
			break;
		case SslMode::Default:
		default:
			break;
	}
	PGconn* conn = PQconnectdb(conninfo.c_str());
	if (!conn) {
		if (m_logger)
			*m_logger << Logger::Level::Error << "PQconnectdb returned null" << std::endl;
		return false;
	}
	if (PQstatus(conn) != CONNECTION_OK) {
		if (m_logger) {
			*m_logger << Logger::Level::Error
					<< "Postgres connection error: "
					<< (PQerrorMessage(conn) ? PQerrorMessage(conn) : "Unknown error")
					<< std::endl;
		}
		PQfinish(conn);
		m_conn = nullptr;
		return false;
	}
	m_conn = conn;
	if (m_logger)
		PQsetNoticeProcessor(conn, PostgresNoticeProcessor, m_logger.get());
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Postgres::DoConnect leave (ok)" << std::endl;
	return true;
}
void Postgres::DoPreDisconnect() noexcept {
	m_prepared_stmts.clear();
}
void Postgres::DoDisconnect() noexcept {
	if (m_conn) {
		PQfinish(static_cast<PGconn*>(m_conn));
		m_conn = nullptr;
	}
}
StormByte::Database::ExpectedRows Postgres::Query(const std::string& query) noexcept {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "Executing query: " << query << std::endl;
	if (!m_connected || !m_conn)
		return Unexpected<ExecuteError>("Database not connected");
	PGresult* res = PQexec(static_cast<PGconn*>(m_conn), query.c_str());
	if (!res)
		return Unexpected<ExecuteError>("Null PGresult");
	ExecStatusType st = PQresultStatus(res);
	if (st != PGRES_TUPLES_OK && st != PGRES_COMMAND_OK) {
		std::string err = PQerrorMessage(static_cast<PGconn*>(m_conn))
						? PQerrorMessage(static_cast<PGconn*>(m_conn))
						: "Unknown Postgres error";
		PQclear(res);
		return Unexpected<ExecuteError>(err);
	}
	ExpectedRows rows = StepResults(res);
	PQclear(res);
	return rows;
}
bool Postgres::SilentQuery(const std::string& query) noexcept {
	return DoSilentQuery(query);
}
bool Postgres::DoSilentQuery(const std::string& query) noexcept {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "Executing silent query: " << query << std::endl;
	if (!m_connected || !m_conn)
		return false;
	PGresult* res = PQexec(static_cast<PGconn*>(m_conn), query.c_str());
	if (!res)
		return false;
	ExecStatusType st = PQresultStatus(res);
	if (st != PGRES_COMMAND_OK && st != PGRES_TUPLES_OK) {
		if (m_logger) {
			*m_logger << Logger::Level::Error
					<< "Postgres SilentQuery error: "
					<< (PQerrorMessage(static_cast<PGconn*>(m_conn))
							? PQerrorMessage(static_cast<PGconn*>(m_conn))
							: "Unknown error")
					<< std::endl;
		}
		PQclear(res);
		return false;
	}
	PQclear(res);
	return true;
}
std::unique_ptr<StormByte::Database::PreparedSTMT>
Postgres::CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept {
	if (!m_conn)
		return nullptr;
	PGconn* conn = static_cast<PGconn*>(m_conn);
	std::string qcopy = query;
	while (!qcopy.empty() &&
		(qcopy.back() == ';' || isspace(static_cast<unsigned char>(qcopy.back())))) {
		qcopy.pop_back();
	}
	PGresult* res = PQprepare(conn, name.c_str(), qcopy.c_str(), 0, nullptr);
	if (!res) {
		if (m_logger) {
			*m_logger << Logger::Level::Error
					<< "PQprepare returned null for statement '" << name << "'"
					<< std::endl;
		}
		return nullptr;
	}
	ExecStatusType st = PQresultStatus(res);
	if (st != PGRES_COMMAND_OK && st != PGRES_TUPLES_OK) {
		if (m_logger) {
			*m_logger << Logger::Level::Error
					<< "PQprepare error for statement '" << name << "': "
					<< (PQresultErrorMessage(res) ? PQresultErrorMessage(res) : "Unknown")
					<< std::endl;
		}
		PQclear(res);
		return nullptr;
	}
	PQclear(res);
	std::unique_ptr<PreparedSTMT> stmt =
		std::make_unique<PreparedSTMT>(PreparedSTMT(std::move(name), std::move(query), m_logger));
	stmt->m_conn = m_conn;
	return stmt;
}
void Postgres::DoBeginTransaction(IsolationLevel level) {
	switch (level) {
		case IsolationLevel::ReadUncommitted:
			DoSilentQuery("BEGIN ISOLATION LEVEL READ UNCOMMITTED;");
			break;
		case IsolationLevel::ReadCommitted:
			DoSilentQuery("BEGIN ISOLATION LEVEL READ COMMITTED;");
			break;
		case IsolationLevel::RepeatableRead:
			DoSilentQuery("BEGIN ISOLATION LEVEL REPEATABLE READ;");
			break;
		case IsolationLevel::Serializable:
			DoSilentQuery("BEGIN ISOLATION LEVEL SERIALIZABLE;");
			break;
		case IsolationLevel::Default:
		default:
			DoSilentQuery("BEGIN;");
			break;
	}
}

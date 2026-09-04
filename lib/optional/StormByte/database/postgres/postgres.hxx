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

#pragma once

#include <StormByte/database/database.hxx>
#include <StormByte/database/postgres/prepared_stmt.hxx>

#include <memory>
#include <string>

struct pg_conn;

/**
 * @brief PostgreSQL backend of the Database module.
 */
namespace StormByte::Database::Postgres {
	/**
	 * @class Postgres
	 * @brief PostgreSQL backend.
	 *
	 * @note Not thread-safe. One instance per thread.
	 * @note Inheritance-oriented. Constructors are protected. SetSslMode() before Connect() if needed.
	 */
	class STORMBYTE_DATABASE_PUBLIC Postgres : public Database {
		public:
			/**
			 * @brief Copy constructor (deleted).
			 */
			Postgres(const Postgres& db) = delete;

			/**
			 * @brief Move constructor.
			 */
			Postgres(Postgres&& db) noexcept = default;

			/**
			 * @brief Copy assignment (deleted).
			 */
			Postgres& operator=(const Postgres& db) = delete;

			/**
			 * @brief Move assignment.
			 */
			Postgres& operator=(Postgres&& db) noexcept = default;

			/**
			 * @brief Destructor.
			 */
			~Postgres() noexcept override;

			/**
			 * @brief Execute a query that returns rows.
			 * @param query SQL text.
			 * @return Result rows or an error.
			 */
			ExpectedRows Query(const std::string& query) noexcept override;

			/**
			 * @brief Execute a query that does not return rows.
			 * @param query SQL text.
			 * @return true on success.
			 */
			bool SilentQuery(const std::string& query) noexcept override;

		protected:
			/**
			 * @brief Connect parameters (copies).
			 * @param host Host name or address.
			 * @param user User name.
			 * @param password Password.
			 * @param db_name Database name.
			 * @param logger Logger instance.
			 */
			Postgres(const std::string& host, const std::string& user, const std::string& password,
					const std::string& db_name, std::shared_ptr<Logger::Log> logger);

			/**
			 * @brief Connect parameters (moved strings).
			 * @param host Host name or address.
			 * @param user User name.
			 * @param password Password.
			 * @param db_name Database name.
			 * @param logger Logger instance.
			 */
			Postgres(std::string&& host, std::string&& user, std::string&& password,
					std::string&& db_name, std::shared_ptr<Logger::Log> logger);

			/**
			 * @brief Internal silent query.
			 * @param query SQL text.
			 * @return true on success.
			 */
			bool DoSilentQuery(const std::string& query) noexcept override;

		private:
			std::string m_host;			///< Host
			std::string m_user;			///< User
			std::string m_password;		///< Password
			std::string m_dbname;		///< Database name
			struct pg_conn* m_conn;		///< Connection handle

			/**
			 * @brief Connect via PQconnectdb.
			 * @return true on success.
			 */
			bool DoConnect() noexcept override;

			/**
			 * @brief Clear prepared statements.
			 */
			void DoPreDisconnect() noexcept override;

			/**
			 * @brief Close the connection.
			 */
			void DoDisconnect() noexcept override;

			/**
			 * @brief Create a PostgreSQL prepared statement (PQprepare).
			 * @param name Statement name.
			 * @param query SQL text.
			 * @return Prepared statement or nullptr.
			 */
			std::unique_ptr<StormByte::Database::PreparedSTMT> CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept override;

			/**
			 * @brief BEGIN with isolation.
			 * @param level Isolation level.
			 */
			void DoBeginTransaction(IsolationLevel level) override;
	};
}

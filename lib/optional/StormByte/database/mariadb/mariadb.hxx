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
#include <StormByte/database/mariadb/prepared_stmt.hxx>

#include <memory>
#include <string>

struct st_mysql;

/**
 * @namespace MariaDB
 * @brief MariaDB / MySQL backend for StormByte::Database.
 */
namespace StormByte::Database::MariaDB {
	/**
	 * @class MariaDB
	 * @brief MariaDB database backend.
	 *
	 * @note Not thread-safe. One instance per thread.
	 *
	 * @note **Inheritance-oriented.** Constructors are protected. Derive your
	 * own class and call the MariaDB constructor from your constructor. Optional
	 * SetSslMode() before Connect(). Not intended for direct generic construction.
	 */
	class STORMBYTE_DATABASE_PUBLIC MariaDB : public Database {
		public:
			/**
			 * Copy constructor (deleted).
			 */
			MariaDB(const MariaDB& db) = delete;

			/**
			 * Move constructor.
			 */
			MariaDB(MariaDB&& db) noexcept = default;

			/**
			 * Copy assignment (deleted).
			 */
			MariaDB& operator=(const MariaDB& db) = delete;

			/**
			 * Move assignment.
			 */
			MariaDB& operator=(MariaDB&& db) noexcept = default;

			/**
			 * Destructor.
			 */
			~MariaDB() noexcept override;

			/**
			 * Executes a query and returns rows.
			 * @param query SQL text.
			 * @return Result rows or an error.
			 */
			ExpectedRows Query(const std::string& query) noexcept override;

			/**
			 * Executes a query that does not return rows.
			 * @param query SQL text.
			 * @return true on success.
			 */
			bool SilentQuery(const std::string& query) noexcept override;

		protected:
			/**
			 * @param host Host name or address.
			 * @param user User name.
			 * @param password Password.
			 * @param db_name Database name.
			 * @param port TCP port.
			 * @param logger Logger instance.
			 */
			MariaDB(const std::string& host, const std::string& user, const std::string& password,
					const std::string& db_name, int port, std::shared_ptr<Logger::Log> logger);

			/**
			 * Move-string overload.
			 * @param host Host name or address.
			 * @param user User name.
			 * @param password Password.
			 * @param db_name Database name.
			 * @param port TCP port.
			 * @param logger Logger instance.
			 */
			MariaDB(std::string&& host, std::string&& user, std::string&& password,
					std::string&& db_name, int port, std::shared_ptr<Logger::Log> logger);

			/**
			 * Internal silent query.
			 * @param query SQL text.
			 * @return true on success.
			 */
			bool DoSilentQuery(const std::string& query) noexcept override;

		private:
			std::string m_host;			///< Host
			std::string m_user;			///< User
			std::string m_password;		///< Password
			std::string m_dbname;		///< Database name
			int m_port;					///< Port
			struct st_mysql* m_conn;	///< Connection handle

			/**
			 * Connects via mysql_real_connect.
			 * @return true on success.
			 */
			bool DoConnect() noexcept override;

			/**
			 * Clears prepared statements.
			 */
			void DoPreDisconnect() noexcept override;

			/**
			 * Closes the connection.
			 */
			void DoDisconnect() noexcept override;

			/**
			 * Creates a MariaDB prepared statement.
			 * @param name Statement name.
			 * @param query SQL text.
			 * @return Prepared statement or nullptr.
			 */
			std::unique_ptr<StormByte::Database::PreparedSTMT> CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept override;

			/**
			 * Sets isolation level and BEGIN.
			 * @param level Isolation level.
			 */
			void DoBeginTransaction(IsolationLevel level) override;
	};
}

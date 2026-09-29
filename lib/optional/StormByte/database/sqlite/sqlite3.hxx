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

#pragma once

#include <StormByte/database/database.hxx>
#include <StormByte/database/sqlite/prepared_stmt.hxx>
#include <StormByte/database/sqlite/telemetry.hxx>

#include <filesystem>
#include <memory>
#include <string_view>

class sqlite3;

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte C++ suite.
 */
namespace StormByte {
	/**
	 * @namespace StormByte::Database
	 * @brief Database module of the StormByte suite.
	 */
	namespace Database {
		/**
		 * @namespace StormByte::Database::SQLite
		 * @brief SQLite backend of the Database module.
		 */
		namespace SQLite {
			/**
			 * @class SQLite3
			 * @brief SQLite3 backend.
			 *
				 * @note Built-in connection operations are serialized. Transactions must stay on their creating thread.
			 * @note Inheritance-oriented. Constructors are protected. Derive and call them from your constructor.
			 */
			class STORMBYTE_DATABASE_PUBLIC SQLite3 : public Database {
				public:
					/**
					 * @brief Copy constructor (deleted).
					 */
					SQLite3(const SQLite3 &db) = delete;

					/**
					 * @brief Move constructor that transfers the database connection.
					 * @param db Database to move from.
					 */
					SQLite3(SQLite3 &&db) noexcept;

					/**
					 * @brief Copy assignment (deleted).
					 */
					SQLite3 &operator=(const SQLite3 &db) = delete;

					/**
					 * @brief Move assignment that transfers the database connection.
					 * @param db Database to move from.
					 * @return Reference to this database.
					 */
					SQLite3 &operator=(SQLite3 &&db) noexcept;

					/**
					 * @brief Destructor.
					 */
					~SQLite3() noexcept override;

					/**
					 * @brief Execute a query that returns rows.
					 * @param query SQL text.
					 * @return Result rows or an error.
					 */
					ExpectedRows Query(std::string_view query) noexcept override;

					/**
					 * @brief Execute a query that does not return rows.
					 * @param query SQL text.
					 * @return true on success.
					 */
					bool SilentQuery(std::string_view query) noexcept override;

				protected:
					/**
					 * @brief In-memory database.
					 * @param logger Logger instance.
					 */
					SQLite3(const StormByte::Shared<Logger::Log>& logger);

					/**
					 * @brief File-backed database.
					 * @param dbfile Path to the database file.
					 * @param logger Logger instance.
					 */
					SQLite3(const std::filesystem::path &dbfile, const StormByte::Shared<Logger::Log>& logger);

					/**
					 * @brief File-backed database (moved path and logger).
					 * @param dbfile Path to the database file.
					 * @param logger Logger instance.
					 */
					SQLite3(std::filesystem::path &&dbfile, const StormByte::Shared<Logger::Log>& logger);

					/**
					 * @brief Enable foreign keys (off by default in SQLite).
					 */
					void EnableForeignKeys();

					/**
					 * @brief Internal silent query.
					 * @param query SQL text.
					 * @return true on success.
					 */
					bool DoSilentQuery(std::string_view query) noexcept override;

				private:
					std::filesystem::path m_database_file; ///< Database file path
					sqlite3 *m_database;				   ///< SQLite handle (incomplete type)

					/**
					 * @brief Open the database and initialize SQLite if needed.
					 * @return true on success.
					 */
					bool DoConnect() noexcept override;

					/**
					 * @brief Clear prepared statements before close.
					 */
					void DoPreDisconnect() noexcept override;

					/**
					 * @brief Close the database handle.
					 */
					void DoDisconnect() noexcept override;

					/**
					 * @brief Decrement global SQLite init refcount / shutdown.
					 */
					void DoPostDisconnect() noexcept override;

					/**
					 * @brief Create a SQLite prepared statement.
					 * @param name Statement name.
					 * @param query SQL text.
					 * @return Prepared statement or nullptr.
					 */
					StormByte::Unique<StormByte::Database::PreparedSTMT> CreatePreparedSTMT(std::string_view name, std::string_view query) noexcept override;

					/**
					 * @brief Map IsolationLevel to BEGIN DEFERRED/IMMEDIATE/EXCLUSIVE.
					 * @param level Isolation level.
					 */
					void DoBeginTransaction(IsolationLevel level) override;
			};
		}	}}

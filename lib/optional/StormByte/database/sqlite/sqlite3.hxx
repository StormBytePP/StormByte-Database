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
#include <StormByte/database/sqlite/prepared_stmt.hxx>

#include <filesystem>
#include <memory>

class sqlite3;

/**
 * @brief SQLite backend of the Database module.
 */
namespace StormByte::Database::SQLite {
	/**
	 * @class SQLite3
	 * @brief SQLite3 backend.
	 *
	 * @note Not thread-safe. One instance per thread.
	 * @note Inheritance-oriented. Constructors are protected. Derive and call them from your constructor.
	 */
	class STORMBYTE_DATABASE_PUBLIC SQLite3 : public Database {
		public:
			/**
			 * @brief Copy constructor (deleted).
			 */
			SQLite3(const SQLite3& db) = delete;

			/**
			 * @brief Move constructor.
			 */
			SQLite3(SQLite3&& db) noexcept = default;

			/**
			 * @brief Copy assignment (deleted).
			 */
			SQLite3& operator=(const SQLite3& db) = delete;

			/**
			 * @brief Move assignment.
			 */
			SQLite3& operator=(SQLite3&& db) noexcept = default;

			/**
			 * @brief Destructor.
			 */
			~SQLite3() noexcept override;

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
			 * @brief In-memory database.
			 * @param logger Logger instance.
			 */
			SQLite3(std::shared_ptr<Logger::Log> logger) noexcept;

			/**
			 * @brief File-backed database.
			 * @param dbfile Path to the database file.
			 * @param logger Logger instance.
			 */
			SQLite3(const std::filesystem::path& dbfile, std::shared_ptr<Logger::Log> logger);

			/**
			 * @brief File-backed database (moved path and logger).
			 * @param dbfile Path to the database file.
			 * @param logger Logger instance.
			 */
			SQLite3(std::filesystem::path&& dbfile, std::shared_ptr<Logger::Log>&& logger);

			/**
			 * @brief Enable foreign keys (off by default in SQLite).
			 */
			void EnableForeignKeys();

			/**
			 * @brief Internal silent query.
			 * @param query SQL text.
			 * @return true on success.
			 */
			bool DoSilentQuery(const std::string& query) noexcept override;

		private:
			std::filesystem::path m_database_file;	///< Database file path
			sqlite3* m_database;					///< SQLite handle (incomplete type)

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
			std::unique_ptr<StormByte::Database::PreparedSTMT> CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept override;

			/**
			 * @brief Map IsolationLevel to BEGIN DEFERRED/IMMEDIATE/EXCLUSIVE.
			 * @param level Isolation level.
			 */
			void DoBeginTransaction(IsolationLevel level) override;
	};
}

/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with StormByte. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <StormByte/database/database.hxx>
#include <StormByte/database/sqlite/prepared_stmt.hxx>

#include <filesystem>
#include <memory>

class sqlite3;

/**
 * @namespace SQLite
 * @brief SQLite backend for StormByte::Database.
 */
namespace StormByte::Database::SQLite {
	/**
	 * @class SQLite3
	 * @brief SQLite3 database backend.
	 *
	 * @note Not thread-safe. One instance per thread.
	 *
	 * @note **Inheritance-oriented.** Constructors are protected. Derive your
	 * own class, call the appropriate SQLite3 constructor from your constructor,
	 * and optionally override lifecycle hooks or call PrepareSTMT / EnableForeignKeys
	 * there. Not intended for direct generic construction.
	 */
	class STORMBYTE_DATABASE_PUBLIC SQLite3 : public Database {
		public:
			/**
			 * Copy constructor (deleted).
			 */
			SQLite3(const SQLite3& db) = delete;

			/**
			 * Move constructor.
			 */
			SQLite3(SQLite3&& db) noexcept = default;

			/**
			 * Copy assignment (deleted).
			 */
			SQLite3& operator=(const SQLite3& db) = delete;

			/**
			 * Move assignment.
			 */
			SQLite3& operator=(SQLite3&& db) noexcept = default;

			/**
			 * Destructor.
			 */
			~SQLite3() noexcept override;

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
			 * In-memory database.
			 * @param logger Logger instance.
			 */
			SQLite3(std::shared_ptr<Logger::Log> logger) noexcept;

			/**
			 * File-backed database.
			 * @param dbfile Path to the database file.
			 * @param logger Logger instance.
			 */
			SQLite3(const std::filesystem::path& dbfile, std::shared_ptr<Logger::Log> logger);

			/**
			 * File-backed database (moved path and logger).
			 * @param dbfile Path to the database file.
			 * @param logger Logger instance.
			 */
			SQLite3(std::filesystem::path&& dbfile, std::shared_ptr<Logger::Log>&& logger);

			/**
			 * Enables foreign key enforcement (off by default in SQLite).
			 */
			void EnableForeignKeys();

			/**
			 * Internal silent query.
			 * @param query SQL text.
			 * @return true on success.
			 */
			bool DoSilentQuery(const std::string& query) noexcept override;

		private:
			std::filesystem::path m_database_file;	///< Database file path
			sqlite3* m_database;					///< SQLite handle (incomplete type)

			/**
			 * Opens the database and initializes SQLite if needed.
			 * @return true on success.
			 */
			bool DoConnect() noexcept override;

			/**
			 * Clears prepared statements before close.
			 */
			void DoPreDisconnect() noexcept override;

			/**
			 * Closes the database handle.
			 */
			void DoDisconnect() noexcept override;

			/**
			 * Decrements global SQLite init refcount / shutdown.
			 */
			void DoPostDisconnect() noexcept override;

			/**
			 * Creates a SQLite prepared statement.
			 * @param name Statement name.
			 * @param query SQL text.
			 * @return Prepared statement or nullptr.
			 */
			std::unique_ptr<StormByte::Database::PreparedSTMT> CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept override;

			/**
			 * Maps IsolationLevel to BEGIN DEFERRED/IMMEDIATE/EXCLUSIVE.
			 * @param level Isolation level.
			 */
			void DoBeginTransaction(IsolationLevel level) override;
	};
}

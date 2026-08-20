#pragma once

#include <StormByte/database/database.hxx>
#include <StormByte/database/sqlite/prepared_stmt.hxx>

#include <filesystem>
#include <memory>

class sqlite3;

/**
 * @namespace SQLite
 * @brief All the classes for handling SQLite databases
 */
namespace StormByte::Database::SQLite {
	/**
	 * @class SQLite3
	 * @brief SQLite3 database class
	 *
	 * @note Not thread-safe. Use one instance per thread for concurrent access.
	 */
	class STORMBYTE_DATABASE_PUBLIC SQLite3 : public Database {
		public:
			/**
			 * Copy constructor (deleted)
			 * @param db Other SQLite3 database to copy from
			 */
			SQLite3(const SQLite3& db) = delete;

			/**
			 * Move constructor
			 * @param db Other SQLite3 database to move from
			 */
			SQLite3(SQLite3&& db) noexcept = default;

			/**
			 * Copy assignment operator (deleted)
			 * @param db Other SQLite3 database to copy from
			 * @return Reference to this SQLite3 database
			 */
			SQLite3& operator=(const SQLite3& db) = delete;

			/**
			 * Move assignment operator
			 * @param db Other SQLite3 database to move from
			 * @return Reference to this SQLite3 database
			 */
			SQLite3& operator=(SQLite3&& db) noexcept = default;

			/**
			 * Destructor
			 */
			~SQLite3() noexcept override;

			/**
			 * Executes a query
			 * @param query The query to execute
			 * @return ExpectedRows containing the result rows or an error
			 */
			ExpectedRows Query(const std::string& query) noexcept override;

			/**
			 * Executes a silent query (no results expected)
			 * @param query The query to execute
			 * @return True if the query was executed successfully
			 */
			bool SilentQuery(const std::string& query) noexcept override;

		protected:
			/**
			 * Constructor (in-memory database)
			 * @param logger Logger instance
			 */
			SQLite3(std::shared_ptr<Logger::Log> logger) noexcept;

			/**
			 * Constructor
			 * @param dbfile database file
			 * @param logger Logger instance
			 */
			SQLite3(const std::filesystem::path& dbfile, std::shared_ptr<Logger::Log> logger);

			/**
			 * Constructor
			 * @param dbfile database file
			 * @param logger Logger instance
			 */
			SQLite3(std::filesystem::path&& dbfile, std::shared_ptr<Logger::Log>&& logger);

			/**
			 * Enable the foreign keys for SQLite3 (default is disabled)
			 */
			void EnableForeignKeys();

			/**
			 * Internal silent query helper
			 * @param query The query to execute
			 * @return True if the query was executed successfully
			 */
			bool DoSilentQuery(const std::string& query) noexcept override;

		private:
			/**
			 * Database file
			 */
			std::filesystem::path m_database_file;

			/**
			 * SQLite3 database handle
			 * (cannot use std::unique_ptr because sqlite3 is an incomplete type)
			 */
			sqlite3* m_database;

			/**
			 * Connects to the database
			 * @return true if connection was successful, false otherwise
			 */
			bool DoConnect() noexcept override;

			/**
			 * Pre-disconnect cleanup
			 */
			void DoPreDisconnect() noexcept override;

			/**
			 * Disconnects from the database
			 */
			void DoDisconnect() noexcept override;

			/**
			 * Post-disconnect cleanup (global SQLite refcount)
			 */
			void DoPostDisconnect() noexcept override;

			/**
			 * Creates a prepared statement
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 * @return The created prepared statement
			 */
			std::unique_ptr<StormByte::Database::PreparedSTMT> CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept override;

			/**
			 * Starts a transaction with the given isolation level
			 * @param level Isolation level
			 */
			void DoBeginTransaction(IsolationLevel level) override;
	};
}

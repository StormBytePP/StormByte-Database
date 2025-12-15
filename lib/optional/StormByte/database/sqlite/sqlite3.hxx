#pragma once

#include <StormByte/database/database.hxx>
#include <StormByte/database/sqlite/prepared_stmt.hxx>

#include <filesystem>
#include <list>
#include <map>
#include <memory>

class sqlite3; // Forward declaration so we don't have to depend on sqlite3 headers

/**
 * @namespace SQLite
 * @brief All the classes for handling SQLite databases
 */
namespace StormByte::Database::SQLite {
	/**
	 * @class SQLite3
	 * @brief SQLite3 database class
	 */
	class STORMBYTE_DATABASE_PUBLIC SQLite3: public Database {
		public:
			/**
			 * Copy constructor
			 */
			SQLite3(const SQLite3& db) 								= delete;

			/**
			 * Move constructor
			 */
			SQLite3(SQLite3&& db) noexcept							= default;

			/**
			 * Assignment operator
			 */
			SQLite3& operator=(const SQLite3& db) 					= delete;

			/**
			 * Move operator
			 */
			SQLite3& operator=(SQLite3&& db) noexcept 				= default;

			/**
			 * Destructor
			 */
			virtual ~SQLite3() noexcept;

			/**
			 * Executes a query
			 * @param query The query to execute.
			 * @return ExpectedRows containing the result rows or an error.
			 */
			ExpectedRows 											Query(const std::string& query) noexcept override;

			/**
			 * Executes a silent query (no results expected)
			 * @param query The query to execute.
			 * @return True if the query was executed successfully
			 */
			bool 													SilentQuery(const std::string& query) noexcept override;

		protected:
			/**
			 * Constructor
			 */
			SQLite3(std::shared_ptr<Logger::Log> logger) noexcept;

			/**
			 * Constructor
			 * @param dbfile database file
			 */
			SQLite3(const std::filesystem::path& dbfile, std::shared_ptr<Logger::Log> logger);

			/**
			 * Constructor
			 * @param dbfile database file
			 */
			SQLite3(std::filesystem::path&& dbfile, std::shared_ptr<Logger::Log>&& logger);

			/**
			 * Enable the foreign keys for SQLite3 (default is disabled)
			 */
			void 													EnableForeignKeys();

		private:
			/**
			 * Database file
			 */
			std::filesystem::path m_database_file;					///< Database file

			/**
			 * SQLite3 database
			 * (can not use std::unique_ptr because sqlite3 is an incomplete type)
			 */
			sqlite3* m_database;									///< SQLite3 database

			/**
			 * Internal function for connecting
			 * @return true if connection was successful, false otherwise
			 */
			bool 													DoConnect() noexcept override;

			/**
			 * Internal function for pre-disconnection setup
			 */
			void 													DoPreDisconnect() noexcept override;

			/**
			 * Internal function for disconnecting
			 */
			void 													DoDisconnect() noexcept override;

			/**
			 * Internal function for post-disconnection setup
			 */
			void 													DoPostDisconnect() noexcept override;

			/**
			 * Creates a prepared statement
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 * @return The created prepared statement
			 */
			std::unique_ptr<StormByte::Database::PreparedSTMT> 		CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept override;
	};
}

#pragma once

#include <StormByte/database/database.hxx>
#include <StormByte/database/sqlite/preparedSTMT.hxx>
#include <StormByte/database/sqlite/query.hxx>
#include <StormByte/database/sqlite/exception.hxx>

#include <filesystem>
#include <list>
#include <map>
#include <memory>

class sqlite3; // Forward declaration so we don't have to depend on sqlite3 headers
/**
 * @namespace Database::SQLite
 * @brief All the classes for handling SQLite databases
 */
namespace StormByte::Database::SQLite {
	/**
	 * @class SQLite3
	 * @brief SQLite3 database class
	 */
	class STORMBYTE_DATABASE_PUBLIC SQLite3: public Database<Query, PreparedSTMT> {
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

		protected:
			/**
			 * Constructor
			 */
			SQLite3();

			/**
			 * Constructor
			 * @param dbfile database file
			 */
			SQLite3(const std::filesystem::path& dbfile);

			/**
			 * Constructor
			 * @param dbfile database file
			 */
			SQLite3(std::filesystem::path&& dbfile);

			/**
			 * Connects to the database.
			 */
			void 												Connect() override;

			/**
			 * Disconnects from the database.
			 */
			void 												Disconnect() override;

			/**
			 * Gets the last error
			 * @return last error
			 */
			const std::string 									LastError() const override;

		private:
			/**
			 * Database file
			 */
			std::filesystem::path m_database_file;	///< Database file

			/**
			 * SQLite3 database
			 * (can not use std::unique_ptr because sqlite3 is an incomplete type)
			 */
			sqlite3* m_database;					///< SQLite3 database

			/**
			 * Enable the foreign keys for SQLite3 (default is disabled)
			 */
			void 												EnableForeignKeys();

			/**
			 * Internal function for disconnecting
			 */
			void												InternalDisconnect();

			/**
			 * Prepares a statement
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 * @return The created prepared statement
			 */
			std::unique_ptr<PreparedSTMT>						InternalPrepare(const std::string& name, const std::string& query) override;

			/**
			 * Executes a query
			 * @param query The query to execute.
			 * @return The created query
			 */
			std::unique_ptr<Query>								InternalQuery(const std::string& query) override;
	};
}

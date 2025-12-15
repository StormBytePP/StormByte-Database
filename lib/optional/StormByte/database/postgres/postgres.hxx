#pragma once

#include <StormByte/database/database.hxx>
#include <StormByte/database/postgres/prepared_stmt.hxx>

#include <filesystem>
#include <memory>

struct pg_conn; ///< Forward declaration of libpq connection struct

/**
 * @namespace Postgres
 * @brief All the classes for handling PostgreSQL databases
 */
namespace StormByte::Database::Postgres {
	/**
	 * @class Postgres
	 * @brief PostgreSQL database implementation
	 */
	class STORMBYTE_DATABASE_PUBLIC Postgres: public Database {
		public:
			/**
			 * Default copy constructor (deleted)
			 * @param db Other Postgres database to copy from
			 */
			Postgres(const Postgres& db) 							= delete;

			/**
			 * Default move constructor
			 * @param db Other Postgres database to move from
			 */
			Postgres(Postgres&& db) noexcept 						= default;

			/**
			 * Default copy assignment operator (deleted)
			 * @param db Other Postgres database to copy from
			 * @return Reference to this Postgres database
			 */
			Postgres& operator=(const Postgres& db) 				= delete;

			/**
			 * Default move assignment operator
			 * @param db Other Postgres database to move from
			 * @return Reference to this Postgres database
			 */
			Postgres& operator=(Postgres&& db) noexcept 			= default;

			/**
			 * Destructor
			 */
			virtual ~Postgres() noexcept;

			/**
			 * Executes a query on the database
			 * @param query The query to execute
			 * @return The resulting rows or an error
			 */
			ExpectedRows 											Query(const std::string& query) noexcept override;

			/**
			 * Executes a query on the database without returning results
			 * @param query The query to execute
			 * @return true on success, false on failure
			 */
			bool 													SilentQuery(const std::string& query) noexcept override;

		protected:
			/**
			 * Constructor
			 * @param host The database host
			 * @param user The database user
			 * @param password The database password
			 * @param db_name The database name
			 * @param logger Logger instance
			 */
			Postgres(const std::string& host, const std::string& user, const std::string& password, const std::string& db_name, std::shared_ptr<Logger::Log> logger);

			/**
			 * Constructor moving strings
			 * @param host The database host
			 * @param user The database user
			 * @param password The database password
			 * @param db_name The database name
			 * @param logger Logger instance
			 */
			Postgres(std::string&& host, std::string&& user, std::string&& password, std::string&& db_name, std::shared_ptr<Logger::Log> logger);

		private:
			std::string m_host; 									///< host
			std::string m_user; 									///< user
			std::string m_password; 								///< password
			std::string m_dbname; 									///< database name
			struct pg_conn* m_conn;									///< Pointer to the Postgres connection

			/**
			 * Connects to the database
			 * @return true on success, false on failure
			 */
			bool 													DoConnect() noexcept override;

			/**
			 * Pre-disconnect operations
			 */
			void 													DoPreDisconnect() noexcept override;

			/**
			 * Disconnects from the database
			 */
			void 													DoDisconnect() noexcept override;

			/**
			 * Creates a prepared statement
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 * @return Unique pointer to the prepared statement
			 */
			std::unique_ptr<StormByte::Database::PreparedSTMT> 		CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept override;
	};
}

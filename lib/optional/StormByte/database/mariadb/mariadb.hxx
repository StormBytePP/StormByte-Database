#pragma once

#include <StormByte/database/database.hxx>
#include <StormByte/database/mariadb/prepared_stmt.hxx>

#include <memory>
#include <string>

struct st_mysql;

/**
 * @namespace MariaDB
 * @brief All the classes for handling MariaDB databases
 */
namespace StormByte::Database::MariaDB {
	/**
	 * @class MariaDB
	 * @brief MariaDB database implementation
	 */
    class STORMBYTE_DATABASE_PUBLIC MariaDB : public Database {
		public:
			MariaDB(const MariaDB& db) 											= delete;
			MariaDB(MariaDB&& db) noexcept 										= default;
			MariaDB& operator=(const MariaDB& db) 								= delete;
			MariaDB& operator=(MariaDB&& db) noexcept 							= default;
			virtual ~MariaDB() noexcept;

			ExpectedRows 														Query(const std::string& query) noexcept override;
			bool 																SilentQuery(const std::string& query) noexcept override;

		protected:
			/**
			 * Constructor
			 * @param host The database host
			 * @param user The database user
			 * @param password The database password
			 * @param db_name The database name
			 * @param port The database port
			 * @param logger Logger instance
			 */
			MariaDB(const std::string& host, const std::string& user, const std::string& password, const std::string& db_name, int port, std::shared_ptr<Logger::Log> logger);

			/**
			 * Constructor moving strings
			 * @param host The database host
			 * @param user The database user
			 * @param password The database password
			 * @param db_name The database name
			 * @param port The database port
			 * @param logger Logger instance
			 */
			MariaDB(std::string&& host, std::string&& user, std::string&& password, std::string&& db_name, int port, std::shared_ptr<Logger::Log> logger);

		private:
			std::string m_host;													///< Database host
			std::string m_user;													///< Database user
			std::string m_password;												///< Database password
			std::string m_dbname;												///< Database name
			int m_port;															///< Database port
			struct st_mysql* m_conn;											///< MariaDB connection handle

			/**
			 * Connects to the database
			 * @return true on success, false on failure
			 */
			bool 																DoConnect() noexcept override;

			/**
			 * Pre-disconnect operations
			 */
			void 																DoPreDisconnect() noexcept override;

			/**
			 * Disconnects from the database
			 */
			void 																DoDisconnect() noexcept override;

			/**
			 * Creates a prepared statement
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 * @return The prepared statement or nullptr on failure
			 */
			std::unique_ptr<StormByte::Database::PreparedSTMT> 					CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept override;
		};
}

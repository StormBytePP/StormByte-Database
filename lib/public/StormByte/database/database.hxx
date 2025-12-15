#pragma once

#include <StormByte/database/prepared_stmt.hxx>
#include <StormByte/database/rows.hxx>
#include <StormByte/logger/log.hxx>

#include <unordered_map>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class Database
	 * @brief Abstract database class for database handling
	 */
	class STORMBYTE_DATABASE_PUBLIC Database {
		public:
			/**
			 * @brief Default constructor.
			 */
			Database(std::shared_ptr<Logger::Log> logger) noexcept:
			m_logger(logger), m_connected(false) {};

			/**
			 * @brief Default copy constructor (deleted)
			 * @param other Other Database to copy from
			 */
			Database(const Database& other)								= delete;

			/**
			 * @brief Default move constructor
			 * @param other Other Database to move from
			 */
			Database(Database&& other)									= default;

			/**
			 * @brief Default copy assignment operator (deleted)
			 * @param other Other Database to copy from
			 * @return Reference to this Database
			 */
			Database& operator=(const Database& other)					= delete;

			/**
			 * @brief Default move assignment operator
			 * @param other Other Database to move from
			 * @return Reference to this Database
			 */
			Database& operator=(Database&& other)						= default;

			/**
			 * @brief Default destructor.
			 */
			virtual ~Database()											= default;

			/**
			 * @brief Connects to the database.
			 */
			bool 														Connect() noexcept;

			/**
			 * @brief Disconnects from the database.
			 */
			virtual void 												Disconnect() noexcept;

			/**
			 * @brief Checks if the database is connected.
			 * @return true if connected, false otherwise
			 */
			inline bool 												IsConnected() const noexcept {
				return m_connected;
			}

			/**
			 * Executes a prepared statement
			 * @tparam Args Types of the arguments
			 * @param name The name of the prepared statement
			 * @param args Arguments to bind and execute
			 * @return Resulting rows
			 */
			template<typename... Args>
			inline ExpectedRows 										ExecuteSTMT(const std::string& name, Args&&... args) {
				auto it = m_prepared_stmts.find(name);
				if (it == m_prepared_stmts.end())
					return Unexpected<UnknownSTMT>(name);
				return it->second->Execute(std::forward<Args>(args)...);
			}

			/**
			 * Executes a query
			 * @param query The query to execute.
			 * @return The created query
			 */
			virtual ExpectedRows										Query(const std::string& query) = 0;

			/**
			 * Executes a query without returning any result
			 * @param query The query to execute.
			 * @return True if the query was executed successfully
			 */
			virtual bool 												SilentQuery(const std::string& query) noexcept = 0;

			/**
			 * Begins a transaction
			 */
			inline virtual void 										BeginTransaction() {
				SilentQuery("BEGIN TRANSACTION;");
			}

			/**
			 * Begins an exclusive transaction
			 */
			inline virtual void 										BeginExclusiveTransaction() {
				SilentQuery("BEGIN EXCLUSIVE TRANSACTION;");
			}

			/**
			 * Commits the transaction
			 */
			inline virtual void 										CommitTransaction() {
				SilentQuery("COMMIT;");
			}

			/**
			 * Rolls back the transaction
			 */
			inline virtual void 										RollbackTransaction() {
				SilentQuery("ROLLBACK;");
			}

		protected:
			std::shared_ptr<Logger::Log> m_logger;						///< Logger instance
			std::unordered_map<
				std::string,
				std::unique_ptr<PreparedSTMT>
			> m_prepared_stmts;											///< Prepared statements
			bool m_connected;											///< Connection state

			/**
			 * @brief Pre-connect action
			 * Default is a noop
			 */
			virtual void 												DoPreConnect() noexcept {}

			/**
			 * @brief Connects to the database
			 * @return true if connection was successful, false otherwise
			 */
			virtual bool 												DoConnect() noexcept = 0;

			/**
			 * @brief Post-connect action
			 * Default is a noop
			 */
			virtual void 												DoPostConnect() noexcept {}

			/**
			 * @brief Pre-disconnect action
			 * Default is a noop
			 */
			virtual void 												DoPreDisconnect() noexcept {}

			/**
			 * @brief Disconnects from the database
			 */
			virtual void 												DoDisconnect() noexcept = 0;

			/**
			 * @brief Post-disconnect action
			 * Default is a noop
			 */
			virtual void 												DoPostDisconnect() noexcept {}

			/**
			 * @brief Creates a prepared statement
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 * @return The created prepared statement
			 */
			virtual std::unique_ptr<PreparedSTMT>						CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept = 0;

			/**
			 * Prepares a statement
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 * @return The created prepared statement
			 */
			void 														PrepareSTMT(std::string&& name, std::string&& query) noexcept;
	};
}
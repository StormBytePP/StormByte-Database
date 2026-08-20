#pragma once

#include <StormByte/database/prepared_stmt.hxx>
#include <StormByte/database/rows.hxx>
#include <StormByte/database/transaction.hxx>
#include <StormByte/database/typedefs.hxx>
#include <StormByte/logger/log.hxx>

#include <memory>
#include <unordered_map>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class Database
	 * @brief Abstract database class for database handling.
	 *
	 * @note This class is **not thread-safe**. Concurrent access from multiple
	 * threads on the same instance is undefined behaviour. For concurrent
	 * workloads create one Database (connection) instance per thread and let
	 * the underlying engine handle concurrency.
	 */
	class STORMBYTE_DATABASE_PUBLIC Database {
		public:
			/**
			 * Constructor
			 * @param logger Logger instance
			 */
			Database(std::shared_ptr<Logger::Log> logger) noexcept
				: m_logger(std::move(logger)), m_connected(false), m_ssl_mode(SslMode::Default) {}

			/**
			 * Copy constructor (deleted)
			 */
			Database(const Database&) = delete;

			/**
			 * Move constructor
			 */
			Database(Database&&) noexcept = default;

			/**
			 * Copy assignment (deleted)
			 */
			Database& operator=(const Database&) = delete;

			/**
			 * Move assignment
			 */
			Database& operator=(Database&&) noexcept = default;

			/**
			 * Destructor
			 */
			virtual ~Database() = default;

			/**
			 * Connects to the database
			 * @return true on success, false otherwise
			 */
			bool Connect() noexcept;

			/**
			 * Disconnects from the database
			 */
			void Disconnect() noexcept;

			/**
			 * Checks if the database is connected
			 * @return true if connected, false otherwise
			 */
			bool IsConnected() const noexcept { return m_connected; }

			/**
			 * Sets the TLS policy applied on the next Connect().
			 * Ignored by SQLite.
			 * @param mode Desired SSL mode
			 */
			void SetSslMode(SslMode mode) noexcept { m_ssl_mode = mode; }

			/**
			 * @return Current TLS policy
			 */
			SslMode GetSslMode() const noexcept { return m_ssl_mode; }

			/**
			 * Executes a prepared statement
			 * @tparam Args Types of the arguments
			 * @param name The name of the prepared statement
			 * @param args Arguments to bind and execute
			 * @return Resulting rows
			 */
			template<typename... Args>
			ExpectedRows ExecuteSTMT(const std::string& name, Args&&... args) {
				auto it = m_prepared_stmts.find(name);
				if (it == m_prepared_stmts.end())
					return Unexpected<UnknownSTMT>(name);
				return it->second->Execute(std::forward<Args>(args)...);
			}

			/**
			 * Executes a query
			 * @param query The query to execute
			 * @return Resulting rows or an error
			 */
			virtual ExpectedRows Query(const std::string& query) = 0;

			/**
			 * Executes a query without returning any result
			 * @param query The query to execute
			 * @return true on success, false on failure
			 */
			virtual bool SilentQuery(const std::string& query) noexcept = 0;

			/**
			 * Begins a transaction with the specified isolation level
			 * @param level Isolation level to use
			 * @return RAII Transaction object
			 */
			Transaction BeginTransaction(IsolationLevel level = IsolationLevel::Default);

			/**
			 * Commits the current transaction
			 */
			void CommitTransaction();

			/**
			 * Rolls back the current transaction
			 */
			void RollbackTransaction();

		protected:
			friend class Transaction;

			std::shared_ptr<Logger::Log> m_logger;													///< Logger instance
			std::unordered_map<std::string, std::unique_ptr<PreparedSTMT>> m_prepared_stmts;			///< Prepared statements
			bool m_connected;																		///< Connection state
			SslMode m_ssl_mode;																		///< TLS policy for network backends

			/**
			 * @name Hooks
			 * @brief Lifecycle hooks called by Connect() / Disconnect().
			 *
			 * When overriding these methods prefer the internal helpers
			 * DoSilentQuery() and DoPrepareSTMT() so that logging and error
			 * handling stay consistent. Using the public SilentQuery() /
			 * PrepareSTMT() from a hook is also safe (there is no mutex).
			 *
			 * @{
			 */

			/**
			 * Pre-connect action. Default is a no-op.
			 */
			virtual void DoPreConnect() noexcept {}

			/**
			 * Connects to the database (backend-specific)
			 * @return true on success, false otherwise
			 */
			virtual bool DoConnect() noexcept = 0;

			/**
			 * Post-connect action. Default is a no-op.
			 */
			virtual void DoPostConnect() noexcept {}

			/**
			 * Pre-disconnect action. Default is a no-op.
			 */
			virtual void DoPreDisconnect() noexcept {}

			/**
			 * Disconnects from the database (backend-specific)
			 */
			virtual void DoDisconnect() noexcept = 0;

			/**
			 * Post-disconnect action. Default is a no-op.
			 */
			virtual void DoPostDisconnect() noexcept {}

			/** @} */

			/**
			 * Creates a prepared statement (backend-specific)
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 * @return The created prepared statement or nullptr on failure
			 */
			virtual std::unique_ptr<PreparedSTMT> CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept = 0;

			/**
			 * Prepares a statement
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 */
			void PrepareSTMT(std::string&& name, std::string&& query) noexcept;

			/**
			 * Internal prepare helper (same as PrepareSTMT; kept for symmetry with hooks).
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 */
			void DoPrepareSTMT(std::string&& name, std::string&& query) noexcept;

			/**
			 * Internal method to start a transaction with the given isolation level.
			 * @param level Isolation level
			 */
			virtual void DoBeginTransaction(IsolationLevel level) = 0;

			/**
			 * Internal silent query helper.
			 * @param query The query to execute
			 * @return true on success, false on failure
			 */
			virtual bool DoSilentQuery(const std::string& query) noexcept = 0;
	};
}

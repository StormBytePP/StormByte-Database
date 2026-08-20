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
	 * @brief Abstract base for database backends.
	 *
	 * @note **Not thread-safe.** Concurrent use of the same instance from multiple
	 * threads is undefined behaviour. Create one connection per thread.
	 *
	 * @note **Inheritance-oriented API.** Concrete backends (SQLite3, MariaDB,
	 * Postgres) expose only protected constructors. The intended usage is to
	 * derive your own class, call the backend constructor from your constructor,
	 * and optionally override lifecycle hooks (DoPreConnect, DoPostConnect, …)
	 * or prepare statements there. Direct “generic” construction of backend
	 * instances is intentionally unsupported.
	 */
	class STORMBYTE_DATABASE_PUBLIC Database {
		public:
			/**
			 * Constructor.
			 * @param logger Logger instance (may be null).
			 */
			Database(std::shared_ptr<Logger::Log> logger) noexcept
				: m_logger(std::move(logger)), m_connected(false), m_ssl_mode(SslMode::Default) {}

			/**
			 * Copy constructor (deleted).
			 */
			Database(const Database&) = delete;

			/**
			 * Move constructor.
			 */
			Database(Database&&) noexcept = default;

			/**
			 * Copy assignment (deleted).
			 */
			Database& operator=(const Database&) = delete;

			/**
			 * Move assignment.
			 */
			Database& operator=(Database&&) noexcept = default;

			/**
			 * Destructor.
			 */
			virtual ~Database() = default;

			/**
			 * Connects to the database.
			 * @return true on success, false otherwise.
			 */
			bool Connect() noexcept;

			/**
			 * Disconnects from the database.
			 */
			void Disconnect() noexcept;

			/**
			 * Checks whether the database is connected.
			 * @return true if connected.
			 */
			bool IsConnected() const noexcept {
				return m_connected;
			}

			/**
			 * Sets the TLS policy applied on the next Connect().
			 * Ignored by SQLite.
			 * @param mode Desired SSL mode.
			 */
			void SetSslMode(SslMode mode) noexcept {
				m_ssl_mode = mode;
			}

			/**
			 * @return Current TLS policy.
			 */
			SslMode GetSslMode() const noexcept {
				return m_ssl_mode;
			}

			/**
			 * Executes a prepared statement by name.
			 * @tparam Args Argument types to bind.
			 * @param name Prepared statement name.
			 * @param args Values to bind (positional, 0-based).
			 * @return Result rows or an error.
			 */
			template<typename... Args>
			ExpectedRows ExecuteSTMT(const std::string& name, Args&&... args) {
				auto it = m_prepared_stmts.find(name);
				if (it == m_prepared_stmts.end())
					return Unexpected<UnknownSTMT>(name);
				return it->second->Execute(std::forward<Args>(args)...);
			}

			/**
			 * Executes a query and returns rows.
			 * @param query SQL text.
			 * @return Result rows or an error.
			 */
			virtual ExpectedRows Query(const std::string& query) = 0;

			/**
			 * Executes a query that does not return rows.
			 * @param query SQL text.
			 * @return true on success, false on failure.
			 */
			virtual bool SilentQuery(const std::string& query) noexcept = 0;

			/**
			 * Begins a transaction.
			 * @param level Isolation level (backend-specific mapping).
			 * @return RAII Transaction (rolls back on destruction if not committed).
			 */
			Transaction BeginTransaction(IsolationLevel level = IsolationLevel::Default);

			/**
			 * Commits the current transaction.
			 */
			void CommitTransaction();

			/**
			 * Rolls back the current transaction.
			 */
			void RollbackTransaction();

		protected:
			friend class Transaction;

			std::shared_ptr<Logger::Log> m_logger; ///< Logger instance
			std::unordered_map<std::string, std::unique_ptr<PreparedSTMT>> m_prepared_stmts; ///< Named prepared statements
			bool m_connected; ///< Connection state
			SslMode m_ssl_mode; ///< TLS policy for network backends

			/**
			 * @name Lifecycle hooks
			 * Called by Connect() / Disconnect(). Prefer DoSilentQuery() and
			 * DoPrepareSTMT() from overrides so logging stays consistent.
			 * @{
			 */

			/**
			 * Pre-connect hook. Default no-op.
			 */
			virtual void DoPreConnect() noexcept {}

			/**
			 * Backend-specific connect.
			 * @return true on success.
			 */
			virtual bool DoConnect() noexcept = 0;

			/**
			 * Post-connect hook. Default no-op.
			 */
			virtual void DoPostConnect() noexcept {}

			/**
			 * Pre-disconnect hook. Default no-op.
			 */
			virtual void DoPreDisconnect() noexcept {}

			/**
			 * Backend-specific disconnect.
			 */
			virtual void DoDisconnect() noexcept = 0;

			/**
			 * Post-disconnect hook. Default no-op.
			 */
			virtual void DoPostDisconnect() noexcept {}

			/** @} */

			/**
			 * Creates a backend-specific prepared statement.
			 * @param name Statement name.
			 * @param query SQL text.
			 * @return Prepared statement or nullptr on failure.
			 */
			virtual std::unique_ptr<PreparedSTMT> CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept = 0;

			/**
			 * Registers a prepared statement under @p name.
			 * @param name Statement name.
			 * @param query SQL text.
			 */
			void PrepareSTMT(std::string&& name, std::string&& query) noexcept;

			/**
			 * Same as PrepareSTMT; kept for symmetry with hooks.
			 * @param name Statement name.
			 * @param query SQL text.
			 */
			void DoPrepareSTMT(std::string&& name, std::string&& query) noexcept;

			/**
			 * Backend-specific BEGIN with isolation level.
			 * @param level Isolation level.
			 */
			virtual void DoBeginTransaction(IsolationLevel level) = 0;

			/**
			 * Backend-specific silent query.
			 * @param query SQL text.
			 * @return true on success.
			 */
			virtual bool DoSilentQuery(const std::string& query) noexcept = 0;
	};
}

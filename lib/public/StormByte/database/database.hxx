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

#include <StormByte/database/prepared_stmt.hxx>
#include <StormByte/database/rows.hxx>
#include <StormByte/database/transaction.hxx>
#include <StormByte/database/typedefs.hxx>
#include <StormByte/logger/log.hxx>

#include <memory>
#include <unordered_map>

/**
 * @brief Database module of the StormByte suite.
 */
namespace StormByte::Database {
	/**
	 * @class Database
	 * @brief Abstract backend.
	 *
	 * @note Not thread-safe. One connection per thread.
	 * @note Inheritance-oriented. Concrete backends expose protected constructors. Derive, call the backend constructor, override hooks if needed.
	 */
	class STORMBYTE_DATABASE_PUBLIC Database {
		public:
			/**
			 * @brief Construct with an optional logger.
			 * @param logger Logger instance (may be null).
			 */
			Database(std::shared_ptr<Logger::Log> logger) noexcept
				: m_logger(std::move(logger)), m_connected(false), m_ssl_mode(SslMode::Default) {}

			/**
			 * @brief Copy constructor (deleted).
			 */
			Database(const Database&) = delete;

			/**
			 * @brief Move constructor.
			 */
			Database(Database&&) noexcept = default;

			/**
			 * @brief Copy assignment (deleted).
			 */
			Database& operator=(const Database&) = delete;

			/**
			 * @brief Move assignment.
			 */
			Database& operator=(Database&&) noexcept = default;

			/**
			 * @brief Destructor.
			 */
			virtual ~Database() = default;

			/**
			 * @brief Connect.
			 * @return true on success.
			 */
			bool Connect() noexcept;

			/**
			 * @brief Disconnect.
			 */
			void Disconnect() noexcept;

			/**
			 * @brief Whether the connection is open.
			 * @return true if connected.
			 */
			bool IsConnected() const noexcept {
				return m_connected;
			}

			/**
			 * @brief TLS policy for the next Connect(). Ignored by SQLite.
			 * @param mode Desired SSL mode.
			 */
			void SetSslMode(SslMode mode) noexcept {
				m_ssl_mode = mode;
			}

			/**
			 * @brief Current TLS policy.
			 * @return Mode.
			 */
			SslMode GetSslMode() const noexcept {
				return m_ssl_mode;
			}

			/**
			 * @brief Execute a prepared statement by name.
			 * @tparam Args Bind argument types.
			 * @param name Prepared statement name.
			 * @param args Positional values (0-based).
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
			 * @brief Execute a query that returns rows.
			 * @param query SQL text.
			 * @return Result rows or an error.
			 */
			virtual ExpectedRows Query(const std::string& query) = 0;

			/**
			 * @brief Execute a query that does not return rows.
			 * @param query SQL text.
			 * @return true on success.
			 */
			virtual bool SilentQuery(const std::string& query) noexcept = 0;

			/**
			 * @brief Begin a transaction.
			 * @param level Isolation (backend-specific mapping).
			 * @return RAII Transaction (rollback on destruction if not committed).
			 */
			Transaction BeginTransaction(IsolationLevel level = IsolationLevel::Default);

			/**
			 * @brief Commit the current transaction.
			 */
			void CommitTransaction();

			/**
			 * @brief Roll back the current transaction.
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
			 * Called by Connect() / Disconnect(). Prefer DoSilentQuery() and DoPrepareSTMT() from overrides.
			 * @{
			 */

			/**
			 * @brief Pre-connect hook. Default no-op.
			 */
			virtual void DoPreConnect() noexcept {}

			/**
			 * @brief Backend connect.
			 * @return true on success.
			 */
			virtual bool DoConnect() noexcept = 0;

			/**
			 * @brief Post-connect hook. Default no-op.
			 */
			virtual void DoPostConnect() noexcept {}

			/**
			 * @brief Pre-disconnect hook. Default no-op.
			 */
			virtual void DoPreDisconnect() noexcept {}

			/**
			 * @brief Backend disconnect.
			 */
			virtual void DoDisconnect() noexcept = 0;

			/**
			 * @brief Post-disconnect hook. Default no-op.
			 */
			virtual void DoPostDisconnect() noexcept {}

			/** @} */

			/**
			 * @brief Create a backend prepared statement.
			 * @param name Statement name.
			 * @param query SQL text.
			 * @return Statement or nullptr on failure.
			 */
			virtual std::unique_ptr<PreparedSTMT> CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept = 0;

			/**
			 * @brief Register a prepared statement under @p name.
			 * @param name Statement name.
			 * @param query SQL text.
			 */
			void PrepareSTMT(std::string&& name, std::string&& query) noexcept;

			/**
			 * @brief Same as PrepareSTMT; kept for hook symmetry.
			 * @param name Statement name.
			 * @param query SQL text.
			 */
			void DoPrepareSTMT(std::string&& name, std::string&& query) noexcept;

			/**
			 * @brief Backend BEGIN with isolation.
			 * @param level Isolation level.
			 */
			virtual void DoBeginTransaction(IsolationLevel level) = 0;

			/**
			 * @brief Backend silent query.
			 * @param query SQL text.
			 * @return true on success.
			 */
			virtual bool DoSilentQuery(const std::string& query) noexcept = 0;
	};
}

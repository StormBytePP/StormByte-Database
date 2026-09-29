/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte-Database.
 *
 * StormByte-Database original source is dual-licensed:
 *
 * 1. GNU Lesser General Public License v3.0 (or later)
 *    You may redistribute and/or modify this file under the terms of the
 *    GNU Lesser General Public License as published by the Free Software
 *    Foundation, either version 3 of the License, or (at your option)
 *    any later version.
 *
 * 2. Commercial license
 *    Alternatively, this file may be used under the terms of a commercial
 *    license agreement with the copyright holder
 *    (David C. Manuelda <StormByte@gmail.com>).
 *
 * Both licenses apply only to original StormByte-Database source in this
 * repository. They do not cover other StormByte modules or any third-party
 * material shipped with this repository (including everything under
 * thirdparty/, and in particular the bundled StormByte-Logger tree and
 * the PostgreSQL, MariaDB and SQLite trees), which remain under their own
 * licenses.
 *
 * Neither license grants any patent rights. Any patent licenses required
 * to use this software or third-party components must be obtained separately
 * from the patent holders.
 *
 * StormByte-Database is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with StormByte-Database. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */

#pragma once

#include <StormByte/database/prepared_stmt.hxx>
#include <StormByte/database/rows.hxx>
#include <StormByte/database/transaction.hxx>
#include <StormByte/database/typedefs.hxx>
#include <StormByte/logger/log.hxx>
#include <StormByte/safe_pointers.hxx>

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte C++ suite.
 */
namespace StormByte {
	/**
	 * @namespace StormByte::Database
	 * @brief Database module of the StormByte suite.
	 */
	namespace Database {
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
				Database(const StormByte::Shared<Logger::Log>& logger) noexcept;

				/**
				 * @brief Copy constructor (deleted).
				 */
				Database(const Database &) = delete;

				/**
				 * @brief Move constructor.
				 */
				Database(Database &&other) noexcept;

				/**
				 * @brief Copy assignment (deleted).
				 */
				Database &operator=(const Database &) = delete;

				/**
				 * @brief Move assignment.
				 */
				Database &operator=(Database &&other) noexcept;

				/**
				 * @brief Destructor.
				 */
				virtual ~Database() noexcept;

				/**
				 * @brief Connect.
				 * @return true on success.
				 */
				bool Connect() noexcept;

				/**
				 * @brief Disconnect.
				 * @note Has no effect when the database is already disconnected.
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
				template <typename... Args>
				ExpectedRows ExecuteSTMT(std::string_view name, Args &&...args) {
					PreparedSTMT *statement = FindPreparedSTMT(name);
					if (!statement)
						return Unexpected<UnknownSTMT>(name);
					return statement->Execute(std::forward<Args>(args)...);
				}

				/**
				 * @brief Execute a query that returns rows.
				 * @param query SQL text.
				 * @return Result rows or an error.
				 */
				virtual ExpectedRows Query(std::string_view query) = 0;

				/**
				 * @brief Execute a query that does not return rows.
				 * @param query SQL text.
				 * @return true on success.
				 */
				virtual bool SilentQuery(std::string_view query) noexcept = 0;

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

				bool m_connected;																 ///< Connection state
				SslMode m_ssl_mode;																 ///< TLS policy for network backends

				StormByte::Shared<Logger::Log> m_logger;	///< Shared logger, safe across the DLL boundary

				/**
				 * @brief Destroy all registered statements inside the Database module.
				 */
				void ClearPreparedSTMTs() noexcept;

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
				virtual StormByte::Unique<PreparedSTMT> CreatePreparedSTMT(std::string_view name, std::string_view query) noexcept = 0;

				/**
				 * @brief Register a prepared statement under @p name.
				 * @param name Statement name.
				 * @param query SQL text.
				 */
				void PrepareSTMT(std::string_view name, std::string_view query) noexcept;

				/**
				 * @brief Same as PrepareSTMT; kept for hook symmetry.
				 * @param name Statement name.
				 * @param query SQL text.
				 */
				void DoPrepareSTMT(std::string_view name, std::string_view query) noexcept;

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
				virtual bool DoSilentQuery(std::string_view query) noexcept = 0;

			private:
				std::unordered_map<std::string, StormByte::Unique<PreparedSTMT>> m_prepared_stmts; ///< Statements owned by Database

				/**
				 * @brief Find a prepared statement by name.
				 * @param name Statement name.
				 * @return Statement or nullptr when absent.
				 */
				PreparedSTMT *FindPreparedSTMT(std::string_view name);
		};
	}
}

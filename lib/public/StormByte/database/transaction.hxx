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

#include <StormByte/database/visibility.h>

/**
 * @brief Database module of the StormByte suite.
 */
namespace StormByte::Database {
	class Database;

	/**
	 * @class Transaction
	 * @brief RAII transaction. Rolls back if neither Commit() nor Rollback() ran before destruction.
	 */
	class STORMBYTE_DATABASE_PUBLIC Transaction {
		public:
			/**
			 * @brief Bind to a database connection.
			 * @param db Owning database.
			 */
			explicit Transaction(Database& db) noexcept;

			/**
			 * @brief Copy constructor (deleted).
			 */
			Transaction(const Transaction&) = delete;

			/**
			 * @brief Copy assignment (deleted).
			 */
			Transaction& operator=(const Transaction&) = delete;

			/**
			 * @brief Move constructor.
			 * @param other Source transaction.
			 */
			Transaction(Transaction&& other) noexcept;

			/**
			 * @brief Move assignment.
			 * @param other Source transaction.
			 * @return *this.
			 */
			Transaction& operator=(Transaction&& other) noexcept;

			/**
			 * @brief Destructor. Rolls back if still active.
			 */
			~Transaction() noexcept;

			/**
			 * @brief Commit.
			 */
			void Commit();

			/**
			 * @brief Rollback.
			 */
			void Rollback();

			/**
			 * @brief Whether neither Commit nor Rollback has been called.
			 * @return true if still active.
			 */
			bool IsActive() const noexcept {
				return m_active;
			}

		private:
			Database* m_db;		///< Owning database (nullptr after move)
			bool m_active;		///< true until Commit / Rollback / destructor
	};
}

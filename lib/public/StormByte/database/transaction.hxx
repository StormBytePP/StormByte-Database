#pragma once

#include <StormByte/database/visibility.h>

/**
 * @namespace StormByte::Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	class Database;

	/**
	 * @class Transaction
	 * @brief RAII wrapper for a database transaction.
	 *
	 * Rolls back automatically if neither Commit() nor Rollback() is called
	 * before destruction.
	 */
	class STORMBYTE_DATABASE_PUBLIC Transaction {
		public:
			/**
			 * @param db Owning database.
			 */
			explicit Transaction(Database& db) noexcept;

			/**
			 * Copy constructor (deleted).
			 */
			Transaction(const Transaction&) = delete;

			/**
			 * Copy assignment (deleted).
			 */
			Transaction& operator=(const Transaction&) = delete;

			/**
			 * Move constructor.
			 * @param other Source transaction.
			 */
			Transaction(Transaction&& other) noexcept;

			/**
			 * Move assignment.
			 * @param other Source transaction.
			 * @return *this
			 */
			Transaction& operator=(Transaction&& other) noexcept;

			/**
			 * Destructor. Rolls back if still active.
			 */
			~Transaction() noexcept;

			/**
			 * Commits the transaction.
			 */
			void Commit();

			/**
			 * Rolls back the transaction.
			 */
			void Rollback();

			/**
			 * @return true if neither Commit nor Rollback has been called.
			 */
			bool IsActive() const noexcept {
				return m_active;
			}

		private:
			Database* m_db;		///< Owning database (nullptr after move)
			bool m_active;		///< true until Commit / Rollback / destructor
	};
}

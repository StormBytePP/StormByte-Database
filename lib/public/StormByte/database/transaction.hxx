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
	 * The transaction is automatically rolled back if neither Commit() nor
	 * Rollback() is called before destruction.
	 */
	class STORMBYTE_DATABASE_PUBLIC Transaction {
		public:
			/**
			 * Constructor
			 * @param db Reference to the owning Database
			 */
			explicit Transaction(Database& db) noexcept;

			/**
			 * Copy constructor (deleted)
			 */
			Transaction(const Transaction&) = delete;

			/**
			 * Copy assignment (deleted)
			 */
			Transaction& operator=(const Transaction&) = delete;

			/**
			 * Move constructor
			 * @param other Transaction to move from
			 */
			Transaction(Transaction&& other) noexcept;

			/**
			 * Move assignment
			 * @param other Transaction to move from
			 * @return Reference to this Transaction
			 */
			Transaction& operator=(Transaction&& other) noexcept;

			/**
			 * Destructor. Rolls back the transaction if it is still active.
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
			 * Checks whether the transaction is still active.
			 * @return true if neither Commit nor Rollback has been called
			 */
			bool IsActive() const noexcept { return m_active; }

		private:
			Database* m_db;		///< Owning database (nullptr after move)
			bool m_active;		///< true until Commit/Rollback/destructor
	};
}
